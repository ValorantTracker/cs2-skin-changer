#include "a2x/client_dll.hpp"
#include "a2x/offsets.hpp"

#pragma once

namespace Offsets
{
    inline std::ptrdiff_t dwEntityList = cs2_dumper::offsets::client_dll::dwEntityList;
    inline std::ptrdiff_t dwGameEntitySystem = cs2_dumper::offsets::client_dll::dwEntityList;
    inline std::ptrdiff_t dwGameEntitySystem_highestEntityIndex = cs2_dumper::offsets::client_dll::dwGameEntitySystem_highestEntityIndex;
    inline std::ptrdiff_t dwLocalPlayerController = cs2_dumper::offsets::client_dll::dwLocalPlayerController;
    inline std::ptrdiff_t dwLocalPlayerPawn = cs2_dumper::offsets::client_dll::dwLocalPlayerPawn;

    ///////////////////////////////////////////////////////////

    inline std::ptrdiff_t m_pInventoryServices = cs2_dumper::schemas::client_dll::CCSPlayerController::m_pInventoryServices; // CCSPlayerController_InventoryServices*
    inline std::ptrdiff_t m_unMusicID = cs2_dumper::schemas::client_dll::CCSPlayerController_InventoryServices::m_unMusicID; // uint16

    inline std::ptrdiff_t m_pClippingWeapon = cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_pClippingWeapon; // C_CSWeaponBase*
    inline std::ptrdiff_t m_pWeaponServices = cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_pWeaponServices; // CPlayer_WeaponServices*
    inline std::ptrdiff_t m_hHudModelArms = cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_hHudModelArms; // CHandle<C_CS2HudModelArms>
    inline std::ptrdiff_t m_hOwnerEntity = cs2_dumper::schemas::client_dll::C_BaseEntity::m_hOwnerEntity; // CHandle<C_BaseEntity>
    inline std::ptrdiff_t m_pEntity = cs2_dumper::schemas::client_dll::CEntityInstance::m_pEntity; // CEntityIdentity*
    inline std::ptrdiff_t m_flags = cs2_dumper::schemas::client_dll::CEntityIdentity::m_flags; // uint32

    inline std::ptrdiff_t m_hMyWeapons = cs2_dumper::schemas::client_dll::CPlayer_WeaponServices::m_hMyWeapons; // C_NetworkUtlVectorBase<CHandle<C_BasePlayerWeapon>>
    inline std::ptrdiff_t m_hActiveWeapon = cs2_dumper::schemas::client_dll::CPlayer_WeaponServices::m_hActiveWeapon; // CHandle<C_BasePlayerWeapon>

    inline std::ptrdiff_t m_pGameSceneNode = cs2_dumper::schemas::client_dll::C_BaseEntity::m_pGameSceneNode; // CGameSceneNode*
    inline std::ptrdiff_t m_pChild = cs2_dumper::schemas::client_dll::CGameSceneNode::m_pChild; // CGameSceneNode*
    inline std::ptrdiff_t m_pNextSibling = cs2_dumper::schemas::client_dll::CGameSceneNode::m_pNextSibling; // CGameSceneNode*
    inline std::ptrdiff_t m_pOwner = cs2_dumper::schemas::client_dll::CGameSceneNode::m_pOwner; // CEntityInstance*

    inline std::ptrdiff_t m_bNeedToReApplyGloves = cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_bNeedToReApplyGloves; // bool
    inline std::ptrdiff_t m_EconGloves = cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_EconGloves; // C_EconItemView
    inline std::ptrdiff_t m_bInitialized = cs2_dumper::schemas::client_dll::C_EconItemView::m_bInitialized; // bool
    inline std::ptrdiff_t m_bRestoreCustomMaterialAfterPrecache = cs2_dumper::schemas::client_dll::C_EconItemView::m_bRestoreCustomMaterialAfterPrecache; // bool
    inline std::ptrdiff_t m_iEntityQuality = cs2_dumper::schemas::client_dll::C_EconItemView::m_iEntityQuality; // int32

    inline std::ptrdiff_t m_modelState = cs2_dumper::schemas::client_dll::CSkeletonInstance::m_modelState; // CModelState
    inline std::ptrdiff_t m_MeshGroupMask = cs2_dumper::schemas::client_dll::CModelState::m_MeshGroupMask; // uint64

    inline std::ptrdiff_t m_nSubclassID = cs2_dumper::schemas::client_dll::C_BaseEntity::m_nSubclassID; // uint64
    inline std::ptrdiff_t m_bMeleeWeapon = cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_bMeleeWeapon; // uint64
    inline std::ptrdiff_t m_WeaponType = cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_WeaponType; // CSWeaponType

    inline std::ptrdiff_t m_nFallbackPaintKit = cs2_dumper::schemas::client_dll::C_EconEntity::m_nFallbackPaintKit; // uint32
    inline std::ptrdiff_t m_nFallbackStatTrak = cs2_dumper::schemas::client_dll::C_EconEntity::m_nFallbackStatTrak; // uint32
    inline std::ptrdiff_t m_flFallbackWear = cs2_dumper::schemas::client_dll::C_EconEntity::m_flFallbackWear; // float32
    inline std::ptrdiff_t m_nFallbackSeed = cs2_dumper::schemas::client_dll::C_EconEntity::m_nFallbackSeed; // uint32
    inline std::ptrdiff_t m_OriginalOwnerXuidLow = cs2_dumper::schemas::client_dll::C_EconEntity::m_OriginalOwnerXuidLow; // uint32

    inline std::ptrdiff_t m_AttributeManager = cs2_dumper::schemas::client_dll::C_EconEntity::m_AttributeManager; // C_AttributeContainer
    inline std::ptrdiff_t m_Item = cs2_dumper::schemas::client_dll::C_AttributeContainer::m_Item; // C_EconItemView

    inline std::ptrdiff_t m_AttributeList = cs2_dumper::schemas::client_dll::C_EconItemView::m_AttributeList; // CAttributeList
    inline std::ptrdiff_t m_NetworkedDynamicAttributes = cs2_dumper::schemas::client_dll::C_EconItemView::m_NetworkedDynamicAttributes; // CAttributeList
    inline std::ptrdiff_t m_Attributes = cs2_dumper::schemas::client_dll::CAttributeList::m_Attributes; // C_UtlVectorEmbeddedNetworkVar<CEconItemAttribute>
    inline std::ptrdiff_t m_iItemDefinitionIndex = cs2_dumper::schemas::client_dll::C_EconItemView::m_iItemDefinitionIndex; // uint16
    inline std::ptrdiff_t m_iAccountID = cs2_dumper::schemas::client_dll::C_EconItemView::m_iAccountID; // uint32
    inline std::ptrdiff_t m_szCustomNameOverride = cs2_dumper::schemas::client_dll::C_EconItemView::m_szCustomNameOverride; // char[161]
    inline std::ptrdiff_t m_iItemIDHigh = cs2_dumper::schemas::client_dll::C_EconItemView::m_iItemIDHigh; // uint32

    inline std::ptrdiff_t m_pDirtyModelData = 0xD8;
    inline std::ptrdiff_t m_DrityMeshGroupMask = 0x10;
}