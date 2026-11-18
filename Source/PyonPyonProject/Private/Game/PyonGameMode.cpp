#include "Game/PyonGameMode.h"
#include "Character/PlayerCharacter.h"

//標準のプレイヤークラスを設定
APyonGameMode::APyonGameMode()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
}
