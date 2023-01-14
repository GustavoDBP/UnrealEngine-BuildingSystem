#pragma once

UENUM(BlueprintType)
enum EConstructorPawnModes {
    None                UMETA(DisplayName="None"),
    WallConstruct           UMETA(DisplayName="Construct"),
    Edit                UMETA(DisplayName="Edit"),
};