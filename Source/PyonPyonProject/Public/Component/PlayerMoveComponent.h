#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerMoveComponent.generated.h"

//着地・ジャンプの通知
DECLARE_MULTICAST_DELEGATE(FOnPlayerLanded);
DECLARE_MULTICAST_DELEGATE(FOnPlayerJumped);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PYONPYONPROJECT_API UPlayerMoveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerMoveComponent();

	//着地時に登録先へ知らせる通知
	FOnPlayerLanded OnPlayerLanded;
	//ジャンプ成功時に登録先へ知らせる通知
	FOnPlayerJumped OnPlayerJumped;

	void HandleLanded();
	void Move(const FVector2D& Input);
	void RequestJump();
	void StopJump();
	void HandleJumped();
};
