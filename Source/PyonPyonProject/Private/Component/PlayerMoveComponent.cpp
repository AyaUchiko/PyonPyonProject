#include "Component/PlayerMoveComponent.h"
#include "GameFramework/Character.h"

//毎フレームの更新を無効にする
UPlayerMoveComponent::UPlayerMoveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

//着地したことを通知
void UPlayerMoveComponent::HandleLanded()
{
	OnPlayerLanded.Broadcast();
}

//カメラの水平方向を基準に移動
void UPlayerMoveComponent::Move(const FVector2D& Input)
{
	//このコンポーネントを持つキャラクター
	ACharacter* Character = Cast<ACharacter>(GetOwner());

	if (!Character || !Character->GetController())
	{
		return;
	}

	//カメラの左右の向きだけを取り出した回転
	const FRotator CameraYaw(0.0f, Character->GetControlRotation().Yaw, 0.0f);
	//カメラの向きを基準にした水平な前方向
	const FVector Forward = FRotationMatrix(CameraYaw).GetUnitAxis(EAxis::X);
	//カメラの向きを基準にした水平な右方向
	const FVector Right = FRotationMatrix(CameraYaw).GetUnitAxis(EAxis::Y);
	//斜め入力でも速くならないようにする
	const FVector2D MovementInput = Input.GetClampedToMaxSize(1.0f);

	Character->AddMovementInput(Forward, MovementInput.Y);
	Character->AddMovementInput(Right, MovementInput.X);
}

//ジャンプさせる
void UPlayerMoveComponent::RequestJump()
{
	//このコンポーネントを持つキャラクター
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		Character->Jump();
	}
}

//ジャンプ終了
void UPlayerMoveComponent::StopJump()
{
	//このコンポーネントを持つキャラクター
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		Character->StopJumping();
	}
}

//ジャンプが成功したことを通知
void UPlayerMoveComponent::HandleJumped()
{
	OnPlayerJumped.Broadcast();
}

