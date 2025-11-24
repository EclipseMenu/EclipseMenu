#ifdef GEODE_IS_WINDOWS

#include "modules/bot/bot.hpp"
#include "modules/config/config.hpp"
#include "modules/hack/hack.hpp"
#include <memory>
#include <atomic>
#include <Zydis/Zydis.h>
#include <Windows.h>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

namespace eclipse::hacks::Bot {

size_t getWriteSizeFromRIP(uint64_t rip, const CONTEXT* ctx)
{
    uint8_t buffer[16];
    memcpy(buffer, (const void*)rip, sizeof(buffer));

    ZydisDecoder decoder;
    if (ZYAN_FAILED(ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64))) {
        return 0;
    }

    ZydisDecodedInstruction insn;
    ZydisDecodedOperand ops[ZYDIS_MAX_OPERAND_COUNT];

    if (ZYAN_FAILED(ZydisDecoderDecodeFull(&decoder, buffer, sizeof(buffer), &insn, ops))) {
        return 0;
    }

    size_t writeSize = 0;

    const bool isRep = (insn.attributes & ZYDIS_ATTRIB_HAS_REP) != 0;

    if (isRep) {
        const ZydisMnemonic m = insn.mnemonic;
        const bool isMovs = (m == ZYDIS_MNEMONIC_MOVSB ||
                             m == ZYDIS_MNEMONIC_MOVSW ||
                             m == ZYDIS_MNEMONIC_MOVSD ||
                             m == ZYDIS_MNEMONIC_MOVSQ);

        const bool isStos = (m == ZYDIS_MNEMONIC_STOSB ||
                             m == ZYDIS_MNEMONIC_STOSW ||
                             m == ZYDIS_MNEMONIC_STOSD ||
                             m == ZYDIS_MNEMONIC_STOSQ);

        if (isMovs || isStos) {
            const ZyanU16 elementBits = ops[0].size;
            const ZyanU64 elementBytes = elementBits / 8;

            const ZyanU64 count = ctx->Rcx;

            return count * elementBytes;
        }
        else {
            return 0;
        }
    }

    for (uint8_t i = 0; i < insn.operand_count; ++i) {
        const auto& op = ops[i];

        if (op.type != ZYDIS_OPERAND_TYPE_MEMORY)
            continue;

        if (!(op.actions & ZYDIS_OPERAND_ACTION_WRITE))
            continue;

        writeSize += (op.size / 8);
    }

    return writeSize;
}

std::string getLastWindowsErrorAsString() {
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0) {
        return std::string();
    }
    
    LPSTR messageBuffer = nullptr;

    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    
    std::string message(messageBuffer, size);
    
    LocalFree(messageBuffer);
            
    return message;
}

struct MemoryRestore {
    DWORD old;
    uintptr_t addr;
};

struct MemoryEntry {
    uintptr_t addr;
    std::vector<uint8_t> data;
    size_t frame;
};

class MemoryTracker {
public:
    MemoryTracker(size_t pageSize) {
        m_memoryLog.reserve(0x500000);
    }

    void enableCapture() {
        m_captureEnabled.store(false);
        m_memoryRestores.clear();
        std::unordered_set<uintptr_t> pages;
        
        for(auto& pair : m_memoryRegions) {
            uintptr_t pageStart = pair.first & ~(m_pageSize - 1);
            uintptr_t pageEnd = (pair.second - 1) & ~(m_pageSize - 1);

            for (uintptr_t p = pageStart; p <= pageEnd; p += m_pageSize) {
                if(pages.contains(p)) continue;
                pages.insert(p);
                DWORD old;
                if(VirtualProtect((void*)p, m_pageSize, PAGE_READONLY, &old))
                    m_memoryRestores.push_back({old, p});
                else
                    geode::log::error("Failed to VirtualProtect: {}", getLastWindowsErrorAsString());
            }
        }
        m_captureEnabled.store(true);
    }

    void disableCapture() {
        m_captureEnabled.store(false);
        for(auto& restore : m_memoryRestores) {
            DWORD tmp;
            if(!VirtualProtect((void*)restore.addr, m_pageSize, restore.old, &tmp))
                geode::log::error("Failed to VirtualProtect: {}", getLastWindowsErrorAsString());
        }
        m_memoryRestores.clear();
        MemoryRestore pr;
        while(consumePendingRestore(pr)) {
            DWORD tmp;
            if(!VirtualProtect((void*)pr.addr, m_pageSize, pr.old, &tmp))
                geode::log::error("Failed to VirtualProtect: {}", getLastWindowsErrorAsString());
        }
        std::lock_guard<std::mutex> lock(m_pendingMutex);
        m_pendingRestores.clear();
    }

    void addWatchedRegion(uintptr_t start, uintptr_t end) {
        m_memoryRegions.push_back({start, end});
    }

    void clearWatchedRegions() {
        m_memoryRegions.clear();
    }

    void writeMemoryLog(size_t frame, uintptr_t addr, std::vector<uint8_t> data) {
        if (frame == 0 || data.size() == 0 || addr == 0) return;

        MemoryEntry entry;
        entry.addr = addr;
        entry.frame = frame;
        entry.data = std::move(data);

        std::lock_guard<std::mutex> lock(m_memoryLogMutex);
        m_memoryLog.push_back(std::move(entry));
    }

    void savePendingRestore(uintptr_t pageBase, DWORD oldProtect) {
        std::lock_guard<std::mutex> lock(m_pendingMutex);
        m_pendingRestores.push_back({oldProtect, pageBase});
    }


    bool consumePendingRestore(MemoryRestore& out) {
        std::lock_guard<std::mutex> lock(m_pendingMutex);
        if (m_pendingRestores.empty()) return false;
        out = m_pendingRestores.back();
        m_pendingRestores.pop_back();
        return true;
    }

    void restoreToFrame(size_t targetFrame) {
        // make sure we perform the restore without interference
        bool hasCapture = m_captureEnabled.load(std::memory_order_relaxed);
        if(hasCapture)
            disableCapture();
        std::unordered_map<uintptr_t, DWORD> pageOriginalProtect;
        {
            std::lock_guard<std::mutex> lock(m_memoryLogMutex);

            size_t eraseFromIndex = m_memoryLog.size();

            for (size_t i = m_memoryLog.size(); i-- > 0;) {
                MemoryEntry& entry = m_memoryLog[i];

                // if (entry.frame <= std::max(0ULL, targetFrame - 1)) {
                if (entry.frame <= targetFrame) {
                    eraseFromIndex = i + 1;
                    break;
                }
                if (entry.addr == 0 || entry.data.empty()) continue;

                if (!isInWatchedRegion(entry.addr)) {
                    continue;
                }

                uintptr_t startPage = entry.addr & ~(m_pageSize - 1);
                uintptr_t endPage   = (entry.addr + entry.data.size() - 1) & ~(m_pageSize - 1);
                size_t pageCount = (endPage - startPage) / m_pageSize + 1;

                bool canRestore = true;
                for (size_t j = 0; j < pageCount; j++) {
                    uintptr_t p = startPage + j * m_pageSize;
                    MEMORY_BASIC_INFORMATION mbi{};
                    if (VirtualQuery((void*)p, &mbi, sizeof(mbi)) == 0) {
                        canRestore = false;
                        break;
                    }
                    if (mbi.State != MEM_COMMIT) {
                        canRestore = false;
                        break;
                    }
                    if (mbi.Protect & PAGE_GUARD || mbi.Protect == PAGE_NOACCESS) {
                        canRestore = false;
                        break;
                    }

                    if (pageOriginalProtect.find(p) == pageOriginalProtect.end()) {
                        DWORD oldProtect;
                        if (VirtualProtect((void*)p, m_pageSize, PAGE_READWRITE, &oldProtect)) {
                            pageOriginalProtect[p] = oldProtect;
                        } else {
                            geode::log::error("restoreToFrame: VirtualProtect set write failed: {}", getLastWindowsErrorAsString());
                            canRestore = false;
                            break;
                        }
                    }
                }

                if (!canRestore) {
                    geode::log::warn("Skipping restore at %p because page(s) not suitable.", (void*)entry.addr);
                    continue;
                }

                __try {
                    memcpy((void*)entry.addr, entry.data.data(), entry.data.size());
                } __except (EXCEPTION_EXECUTE_HANDLER) {
                    geode::log::error("Exception during memcpy when restoring addr %p (frame %zu)", (void*)entry.addr, entry.frame);
                }
            }

            m_memoryLog.erase(m_memoryLog.begin() + eraseFromIndex, m_memoryLog.end());
        }

        for (auto& [page, old] : pageOriginalProtect) {
            DWORD tmp;
            if (!VirtualProtect((void*)page, m_pageSize, old, &tmp)) {
                geode::log::error("restoreToFrame: VirtualProtect restore failed: {}", getLastWindowsErrorAsString());
            }
        }
        pageOriginalProtect.clear();

        if(hasCapture)
            enableCapture();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(m_memoryLogMutex);
        m_memoryLog.clear();
    }

    bool isInWatchedRegion(uintptr_t addr) const {
        for (auto& pair : m_memoryRegions) {
            if (!pair.first || !pair.second || pair.second <= pair.first) continue;
            if (addr >= pair.first && addr < pair.second) return true;
        }
        return false;
    }

    bool isCaptureEnabled() const {
        return m_captureEnabled.load(std::memory_order_relaxed);
    }

    size_t getPageSize() const { return m_pageSize; }

    size_t getMemoryLogSize() const { return m_memoryLog.size(); }

private:
    std::atomic<bool> m_captureEnabled = false;
    std::mutex m_pendingMutex;
    std::mutex m_memoryLogMutex;
    std::vector<MemoryEntry> m_memoryLog;
    std::vector<MemoryRestore> m_memoryRestores;
    std::vector<MemoryRestore> m_pendingRestores;
    std::vector<std::pair<uintptr_t, uintptr_t>> m_memoryRegions;
    size_t m_pageSize = 0x1000;
};

static size_t s_currentFrame = 0;
static std::unique_ptr<MemoryTracker> s_memoryTracker = nullptr;
static PVOID s_writeHandler = nullptr, s_stepHandler = nullptr;

LONG CALLBACK writeWatchHandler(PEXCEPTION_POINTERS info) {
    if (!s_memoryTracker->isCaptureEnabled())
        return EXCEPTION_CONTINUE_SEARCH;
    auto code = info->ExceptionRecord->ExceptionCode;
    if (code != EXCEPTION_ACCESS_VIOLATION)
        return EXCEPTION_CONTINUE_SEARCH;

    ULONG_PTR faultType = info->ExceptionRecord->ExceptionInformation[0];
    ULONG_PTR faultAddr = info->ExceptionRecord->ExceptionInformation[1];

    if (faultType != 1)  
        return EXCEPTION_CONTINUE_SEARCH;

    uintptr_t a = faultAddr;
    uintptr_t pageBase = a & ~(s_memoryTracker->getPageSize() - 1);

    if(s_memoryTracker->isInWatchedRegion(a)) {
        size_t size = getWriteSizeFromRIP(info->ContextRecord->Rip, info->ContextRecord);
        std::vector<uint8_t> before(size);
        size_t maxReadable = s_memoryTracker->getPageSize() - (faultAddr & (s_memoryTracker->getPageSize() - 1));
        size_t toRead = std::min(size, maxReadable);

        __try {
            memcpy(before.data(), (void*)faultAddr, toRead);
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            return EXCEPTION_CONTINUE_SEARCH;
        }

        s_memoryTracker->writeMemoryLog(s_currentFrame, faultAddr, std::move(before));
    }

    DWORD old;
    if(!VirtualProtect((void*)pageBase, s_memoryTracker->getPageSize(), PAGE_READWRITE, &old))
        geode::log::error("Failed to VirtualProtect: {}", getLastWindowsErrorAsString());

    // single step after the instruction finishes
    info->ContextRecord->EFlags |= 0x100;

    s_memoryTracker->savePendingRestore(pageBase, old);

    return EXCEPTION_CONTINUE_EXECUTION;
}

LONG CALLBACK singleStepHandler(PEXCEPTION_POINTERS info) {
    if (!s_memoryTracker->isCaptureEnabled())
        return EXCEPTION_CONTINUE_SEARCH;
    if (info->ExceptionRecord->ExceptionCode != EXCEPTION_SINGLE_STEP)
        return EXCEPTION_CONTINUE_SEARCH;

    MemoryRestore pr;
    while(s_memoryTracker->consumePendingRestore(pr)) {
        DWORD tmp;
        if(!VirtualProtect((void*)pr.addr, s_memoryTracker->getPageSize(), pr.old, &tmp))
            geode::log::error("Failed to VirtualProtect: {}", getLastWindowsErrorAsString());
    }

    info->ContextRecord->EFlags &= ~0x100;

    return EXCEPTION_CONTINUE_EXECUTION;
}

class $modify(MPFPlayLayer, PlayLayer) {

    static void onModify(auto& self) {
        std::vector<geode::Hook*> hooks;
        for(auto& hook : self.m_hooks) hooks.push_back(hook.second.get());
        
        config::addDelegate("bot.practice-fix-mode", [hooks = std::move(hooks)]{
            if (config::get<int>("bot.practice-fix-mode", 0) == 0) {
                for(auto& hook : hooks) (void) hook->disable();
            } else {
                for(auto& hook : hooks) (void) hook->enable();
            }
        });
    }

    void pauseGame(bool p0) {
        PlayLayer::pauseGame(p0);
        s_memoryTracker->disableCapture();
    }

    void loadFromCheckpoint(CheckpointObject* checkpoint) {
        int botState = config::get<int>("bot.state", 0);
        if (botState != (int)eclipse::bot::State::RECORD)
            return PlayLayer::loadFromCheckpoint(checkpoint);

        PlayLayer::loadFromCheckpoint(checkpoint);
        if(m_checkpointArray->count() > 0)
            s_memoryTracker->restoreToFrame(checkpoint->m_gameState.m_currentProgress);
    }

    void resetLevel() {
        PlayLayer::resetLevel();

        if (m_checkpointArray->count() > 0) return;

        s_currentFrame = 0;
        s_memoryTracker->clear();
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);   
        bool hasCapture = s_memoryTracker->isCaptureEnabled();
        int botState = config::get<int>("bot.state", 0);
        int practiceFixMode = config::get<int>("bot.practice-fix-mode", 0);
        if(practiceFixMode == 1 && s_currentFrame > 0 && m_isPracticeMode && botState == (int)eclipse::bot::State::RECORD && !hasCapture) {
            s_memoryTracker->clearWatchedRegions();
            #define REGION(p,x,y) s_memoryTracker->addWatchedRegion((uintptr_t)p + offsetof(PlayerObject, x), (uintptr_t)p + offsetof(PlayerObject, y));
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Winvalid-offsetof"
            s_memoryTracker->addWatchedRegion((uintptr_t)m_player1 + offsetof(cocos2d::CCNode, m_fRotationX), (uintptr_t)m_player1 + offsetof(cocos2d::CCNode, m_pCamera));
            REGION(m_player1, m_mainLayer, m_currentSlope2);
            REGION(m_player1, m_preLastGroundObject, m_currentPotentialSlope);
            REGION(m_player1, unk_584, m_rotateObjectsRelated);
            REGION(m_player1, m_rotationSpeed, m_ghostTrail);
            REGION(m_player1, m_speedMultiplier, m_ringRelatedSet);
            REGION(m_player1, m_objectSnappedTo, m_jumpBuffered);
            REGION(m_player1, m_stateRingJump, m_holdingButtons);
            REGION(m_player1, m_inputsLocked, m_enable22Changes);

            s_memoryTracker->addWatchedRegion((uintptr_t)m_player2 + offsetof(cocos2d::CCNode, m_fRotationX), (uintptr_t)m_player2 + offsetof(cocos2d::CCNode, m_pCamera));
            REGION(m_player2, m_mainLayer, m_currentSlope2);
            REGION(m_player2, m_preLastGroundObject, m_currentPotentialSlope);
            REGION(m_player2, unk_584, m_rotateObjectsRelated);
            REGION(m_player2, m_rotationSpeed, m_ghostTrail);
            REGION(m_player2, m_speedMultiplier, m_ringRelatedSet);
            REGION(m_player2, m_objectSnappedTo, m_jumpBuffered);
            REGION(m_player2, m_stateRingJump, m_holdingButtons);
            REGION(m_player2, m_inputsLocked, m_enable22Changes);
            #pragma GCC diagnostic pop
            #undef REGION
            s_memoryTracker->enableCapture();
        }
    }

    void togglePracticeMode(bool mode) {
        PlayLayer::togglePracticeMode(mode);
        if(!mode) s_memoryTracker->disableCapture();
    }

    void onQuit() {
        s_memoryTracker->disableCapture();
        s_memoryTracker->clear();
        PlayLayer::onQuit();
    }
};

class $modify(GJBaseGameLayer) {
    void processCommands(float dt) {
        GJBaseGameLayer::processCommands(dt);
        s_currentFrame = m_gameState.m_currentProgress;
    }
};

$on_mod(Loaded) {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    s_memoryTracker = std::make_unique<MemoryTracker>(si.dwPageSize);

    s_writeHandler = AddVectoredExceptionHandler(1, writeWatchHandler);
    s_stepHandler = AddVectoredExceptionHandler(1, singleStepHandler);
}

}

#endif