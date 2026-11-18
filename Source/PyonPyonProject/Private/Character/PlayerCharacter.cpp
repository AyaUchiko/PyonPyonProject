#include "Character/PlayerCharacter.h"
#include "Component/PlayerMoveComponent.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "Engine/World.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputCoreTypes.h"
#include "UObject/ConstructorHelpers.h"

//移動・カメラ・入力アセットの初期設定
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	MoveComponent = CreateDefaultSubobject<UPlayerMoveComponent>(TEXT("PlayerMoveComponent"));

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 450.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 60.0f);
	CameraBoom->bUsePawnControlRotation = true;
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	JumpMaxHoldTime = 0.2f;

	//標準の入力マッピングアセットを読み込む
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingAsset(TEXT("/Game/InputAction/IMC_Default.IMC_Default"));
	//移動用の入力アセットを読み込む
	static ConstructorHelpers::FObjectFinder<UInputAction> MoveAsset(TEXT("/Game/InputAction/IA_Move.IA_Move"));
	//ジャンプ用の入力アセットを読み込む
	static ConstructorHelpers::FObjectFinder<UInputAction> JumpAsset(TEXT("/Game/InputAction/IA_Jump.IA_Jump"));
	DefaultMappingContext = MappingAsset.Object;
	MoveAction = MoveAsset.Object;
	JumpAction = JumpAsset.Object;
}

//開始時に移動通知と入力設定を登録
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	MoveComponent->OnPlayerLanded.AddUObject(this, &APlayerCharacter::HandlePlayerLanded);
	MoveComponent->OnPlayerJumped.AddUObject(this, &APlayerCharacter::HandlePlayerJumped);
	RegisterInputMappingContext();
}

//操作対象になったときに入力設定を登録
void APlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	RegisterInputMappingContext();
}

//ローカルプレイヤーの入力設定を重複せずに登録
void APlayerCharacter::RegisterInputMappingContext()
{

	//このキャラクターを操作するコントローラー
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (!PlayerController)
	{
		return;
	}

	//この端末で操作しているプレイヤー
	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();

	if (!LocalPlayer)
	{
		return;
	}

	//ローカルプレイヤーの入力マッピングを管理する仕組み
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (InputSubsystem && DefaultMappingContext)
	{
		if (!InputSubsystem->HasMappingContext(DefaultMappingContext))
		{
			InputSubsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//着地通知を受け取ってログを表示
void APlayerCharacter::HandlePlayerLanded()
{
	UE_LOG(LogTemp, Display, TEXT("着地通知を受け取りました"));
}

//着地を移動コンポーネントに伝える
void APlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	MoveComponent->HandleLanded();
}

//ジャンプ成功を移動コンポーネントに伝える
void APlayerCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();
	MoveComponent->HandleJumped();
}

//ジャンプ通知を受け取ってログを表示
void APlayerCharacter::HandlePlayerJumped()
{
	UE_LOG(LogTemp, Display, TEXT("Jump notification received"));
}

//移動・ジャンプ・視点操作の入力を割り当てる
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//入力アクションと処理を結び付けるコンポーネント
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCharacter requires EnhancedInputComponent"));
		return;
	}

	if (MoveAction)
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::HandleMove);
	}
	if (JumpAction)
	{
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::HandleJumpStarted);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::HandleJumpStopped);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Canceled, this, &APlayerCharacter::HandleJumpStopped);
	}

	PlayerInputComponent->BindAxisKey(EKeys::MouseX, this, &APlayerCharacter::HandleMouseYaw);
	PlayerInputComponent->BindAxisKey(EKeys::MouseY, this, &APlayerCharacter::HandleMousePitch);
	PlayerInputComponent->BindAxisKey(EKeys::Gamepad_RightX, this, &APlayerCharacter::HandleGamepadYaw);
	PlayerInputComponent->BindAxisKey(EKeys::Gamepad_RightY, this, &APlayerCharacter::HandleGamepadPitch);
}

//ジャンプ開始
void APlayerCharacter::HandleJumpStarted()
{
	MoveComponent->RequestJump();
}

//ジャンプ終了
void APlayerCharacter::HandleJumpStopped()
{
	MoveComponent->StopJump();
}

//マウスで視点を左右に回す
void APlayerCharacter::HandleMouseYaw(float Value)
{
	AddControllerYawInput(Value);
}

//マウスで視点を上下に回す
void APlayerCharacter::HandleMousePitch(float Value)
{
	AddControllerPitchInput(-Value);
}

//スティックで視点を左右に回す
void APlayerCharacter::HandleGamepadYaw(float Value)
{
	AddControllerYawInput(Value * GamepadLookRate * GetWorld()->GetDeltaSeconds());
}

//スティックで視点を上下に回す
void APlayerCharacter::HandleGamepadPitch(float Value)
{
	AddControllerPitchInput(-Value * GamepadLookRate * GetWorld()->GetDeltaSeconds());
}

//移動入力を移動コンポーネントに渡す
void APlayerCharacter::HandleMove(const FInputActionValue& Value)
{
	//移動入力の値（X：左右、Y：前後）
	const FVector2D Input = Value.Get<FVector2D>();
	MoveComponent->Move(Input);
}
