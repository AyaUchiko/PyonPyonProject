#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;
class UPlayerMoveComponent;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class PYONPYONPROJECT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void PawnClientRestart() override;

protected:
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& Hit)override;
	virtual void OnJumped_Implementation() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	//キャラクターとカメラの距離や向きを調整するアーム
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	//プレイヤーを映す追従カメラ
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	//スティック最大入力時の視点回転速度（度／秒）
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (ClampMin = "0.0"))
	float GamepadLookRate = 120.0f;

	//移動・ジャンプの処理と通知を担当するコンポーネント
	UPROPERTY(VisibleAnywhere, Category = "Player")
	TObjectPtr<UPlayerMoveComponent>MoveComponent;

	//移動入力を受け取る入力アクション
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction>MoveAction;

	//キーやボタンと入力アクションの対応設定
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext>DefaultMappingContext;

	//ジャンプ入力を受け取る入力アクション
	UPROPERTY(EditDefaultsOnly,Category="Input")
	TObjectPtr<UInputAction>JumpAction;

private:
	void RegisterInputMappingContext();
	void HandlePlayerLanded();
	void HandlePlayerJumped();
	void HandleMove(const FInputActionValue& Value);
	void HandleJumpStarted();
	void HandleJumpStopped();
	void HandleMouseYaw(float Value);
	void HandleMousePitch(float Value);
	void HandleGamepadYaw(float Value);
	void HandleGamepadPitch(float Value);
};
