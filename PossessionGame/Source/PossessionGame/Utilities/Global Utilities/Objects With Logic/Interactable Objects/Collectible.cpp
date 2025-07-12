#include "Collectible.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/Data Assets/Base Data Assets/CollectibleData.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Utilities/Global Utilities/UI Actors/Marker.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
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
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ACollectible::EnableCollectability);
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
}

void ACollectible::Collect()
{
	UE_LOG(LogTemp, Error, TEXT("COLLECTED %s"), *UEnum::GetValueAsString(Data->GetCollectibleName()));
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
		case EPoolObject::Coin: (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnCoinCollected.ExecuteIfBound(1);
		default: ;
	}
}

void ACollectible::ShowMarker()
{
	if (!Marker) Marker = GetWorld()->SpawnActor<AMarker>(GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->GetMarkerClass(), GetActorLocation() + FVector(0.f, 0.f, 50.f), FRotator::ZeroRotator);
	Marker->Toggle(true);
	Marker->SetActorLocation(GetActorLocation() + FVector(0.f, 0.f, 50.f));
}

void ACollectible::ToggleCollectible(const bool bState, const bool bMarkerIncluded)
{
	if (bState)
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		if (bMarkerIncluded && Data->GetIsMarkerEnabled()) ShowMarker();
	}
	else
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (bMarkerIncluded && Marker && Data->GetIsMarkerEnabled()) Marker->Toggle(false);
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

void ACollectible::EnableCollectability(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	Collect();
}
