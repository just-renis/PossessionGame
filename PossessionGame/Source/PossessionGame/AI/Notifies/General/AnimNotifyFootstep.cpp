#include "AnimNotifyFootstep.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UAnimNotifyFootstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp || !Sound) return;
	const TObjectPtr<AActor> Owner = MeshComp->GetOwner();
	if (!Owner) return;
	TObjectPtr<ACharacter> Character = nullptr;
	if (bIsPlayer)
	{
		if (const TObjectPtr<AWeapon> Weapon = Cast<AWeapon>(Owner)) Character = Weapon->GetProprietor();
	}
	else Character = Cast<ACharacter>(Owner);
	if (!Character || !Character->GetMovementComponent()) return;
	if (Character->GetCharacterMovement()->IsMovingOnGround()) UGameplayStatics::PlaySoundAtLocation(Character, Sound, MeshComp->GetComponentLocation());
}
