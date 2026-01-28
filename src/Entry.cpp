#include "Entry.h"

#include <ll/api/mod/RegisterHelper.h>
#include <ll/api/memory/Hook.h>
#include <ll/api/memory/Signature.h>
#include <ll/api/io/Logger.h>

#include <mc/world/actor/player/Player.h>
#include <mc/world/item/ItemStack.h>
#include <mc/world/actor/Actor.h>
#include <mc/world/level/Level.h>

using namespace ll::memory;
using namespace ll::literals::memory_literals; 

class ActorRuntimeID {
public:
    unsigned long long id;
};

constexpr int InventoryTransactionError_Success = 0;

LL_AUTO_STATIC_HOOK(
    NameTagPlus,
    ll::memory::HookPriority::Normal,
    "48 8B C4 48 89 58 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 40 ? 44 0F 29 48 ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 0F B6 E0"_sig,
    int,            
    void* a1,       
    Player* player, 
    bool isLite     
) {
    if (!player) return origin(a1, player, isLite);

    auto& item = player->getSelectedItem();

    if (!item.isNull() && item.getTypeName() == "minecraft:name_tag" && item.hasCustomHoverName()) {
        
        // 偏移量 104/0x68
        // *(_QWORD *)(a1 + 104)
        ActorRuntimeID targetId = *reinterpret_cast<ActorRuntimeID*>((uintptr_t)a1 + 104);
        auto target = player->getLevel().getRuntimeEntity(targetId, false);
        
        if (target) {
            std::string newName = item.getCustomName();
            
            target->setNameTag(newName);
            target->setNameTagVisible(true);
            target->setPersistent(); 

            return InventoryTransactionError_Success;
        }
    }

    return origin(a1, player, isLite);
}

namespace nametag {

NameTagPlus& NameTagPlus::getInstance() {
    static NameTagPlus instance;
    return instance;
}

bool NameTagPlus::load() {
    getSelf().getLogger().info("NameTagPlus 加载中......");
    getSelf().getLogger().info("Author: PuceLi");
    return true;
}

bool NameTagPlus::enable() {
    getSelf().getLogger().info("NameTagPlus 已启用");
    return true;
}

bool NameTagPlus::disable() {
    getSelf().getLogger().info("NameTagPlus 已禁用");
    return true;
}

} // namespace nametag

LL_REGISTER_MOD(nametag::NameTagPlus, nametag::NameTagPlus::getInstance());