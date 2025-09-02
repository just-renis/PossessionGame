#include "Collectible.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/Data Assets/Base Data Assets/CollectibleData.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/MarkerManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

ACollectible::ACollectible()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("collectibleMesh"));
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("collision"));
	RootComponent = Mesh;
	Collision->SetupAttachment(RootComponent);
}

void ACollectible::BeginPlay()
{
	Super::BeginPlay();
	if (Data->HasAura()) GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(Data->GetAuraName(), nullptr, &Aura, Collision, NAME_None);
	PoolingManager = GetWorld()->GetSubsystem<UPoolingManager>();
}

void ACollectible::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(FloatHandle);
	GetWorldTimerManager().ClearTimer(SpinHandle);
	Super::EndPlay(EndPlayReason);
}

void ACollectible::Init(bool bInIsPlayerObject)
{
	Super::Init(bInIsPlayerObject);
	if (Data->GetNeedSpawnVisualization())
	{
		GetWorldTimerManager().SetTimer(ProcessSpawnHandle, this, &ACollectible::ProcessSpawnVisualization, 0.05f, true);
		if (Mesh->GetNumMaterials() > 0)
		{
			if (!MID_Border) MID_Border = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(0), this);
			if (!MID_Main) MID_Main = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(1), this);
			MID_Border->SetScalarParameterValue("Progress", 0.4f);
			MID_Main->SetScalarParameterValue("Progress", 0.f);
		}
	}
	if (Aura) Aura->Activate();
	if (Data->GetIsFloating())
	{
		InitialSpawnZ = GetActorLocation().Z;
		GetWorldTimerManager().SetTimer(FloatHandle, this, &ACollectible::Float, 0.025f, true);
	}
	if (Data->GetIsSpinning()) GetWorldTimerManager().SetTimer(SpinHandle, this, &ACollectible::Spin, 0.025f, true);
	SetActorRotation(Data->GetRotation());
	ToggleCollectible(true, true);
	EnableCollectCollision();
}

void ACollectible::Collect()
{
	Collision->OnComponentBeginOverlap.RemoveAll(this);
	if (Data->GetNeedSpawnVisualization()) ClearSpawnVisualization();
	if (Aura) Aura->Deactivate();
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UsbCollect);
	GetWorldTimerManager().ClearTimer(FloatHandle);
	GetWorldTimerManager().ClearTimer(SpinHandle);
	PoolingManager->ReturnObjectToPool(Data->GetCollectibleName(), this);
	ToggleCollectible(false, true);
	switch (Data->GetCollectibleName())
	{
		case EPoolObject::Usb: (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnUsbCollected.ExecuteIfBound(); break;
		case EPoolObject::Artifact: (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnArtifactCollected.ExecuteIfBound(); break;
		default: ;
	}
}

void ACollectible::ToggleCollectible(const bool bState, const bool bMarkerIncluded)
{
	if (bState)
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		if (bMarkerIncluded && Data->GetIsMarkerEnabled()) GetWorld()->GetSubsystem<UMarkerManager>()->SpawnMarkerAtLocation(GetActorLocation() + FVector::UpVector * 50.f);
	}
	else
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (bMarkerIncluded && Data->GetIsMarkerEnabled()) GetWorld()->GetSubsystem<UMarkerManager>()->DestroyMarkers();
	}
}

void ACollectible::Float()
{
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = InitialSpawnZ + FMath::Sin(GetWorld()->GetTimeSeconds() * 2.f) * 20.f;
	SetActorLocation(NewLocation);
}

void ACollectible::Spin()
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += Data->GetSpinningSpeed() * 0.025f;
	SetActorRotation(NewRotation);
}

void ACollectible::ProcessSpawnVisualization()
{
	SpawnProcessIndex++;
	if (!MID_Border || !MID_Main) return;
	MID_Border->SetScalarParameterValue("Progress", 0.4f + 0.015f * SpawnProcessIndex);
	MID_Main->SetScalarParameterValue("Progress", 0.05f * SpawnProcessIndex);
	if (SpawnProcessIndex >= 20) ClearSpawnVisualization();
}

void ACollectible::ClearSpawnVisualization()
{
	GetWorldTimerManager().ClearTimer(ProcessSpawnHandle);
	SpawnProcessIndex = 0;
}

void ACollectible::EnableCollectCollision()
{
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ACollectible::EnableCollectAbility);
	// To check if player is in range on spawn.
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ACollectible::EnableCollectAbility(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	Collect();
}
