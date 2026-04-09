#include "TerminalCandle.h"
#include "NiagaraActor.h"
#include "NiagaraComponent.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

ATerminalCandle::ATerminalCandle()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	BallMesh->SetupAttachment(Mesh, FName("BallSocket"));
}

void ATerminalCandle::BeginPlay()
{
	Super::BeginPlay();
	const TObjectPtr<UPrimaryDataAssetsManager> AssetsManager = GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>();
	if (AssetsManager->IsVfxAlreadyLoaded(EVfx::TerminalCandleEnergy)) AssetsManager->AttachVfx(EVfx::TerminalCandleEnergy, nullptr, &EnergyVfx, BallMesh, FName("VfxSocket"));
	else
	{
		TWeakObjectPtr WeakThis = this;
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->LoadVfx(EVfx::TerminalCandleEnergy, [WeakThis]
		{
			if (!WeakThis.IsValid() || !WeakThis.Get()) return;
			if (WeakThis->EnergyVfx)
			{
				WeakThis->EnergyVfx->DestroyComponent();
				WeakThis->EnergyVfx = nullptr;
			}
			WeakThis->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::TerminalCandleEnergy, nullptr, &WeakThis->EnergyVfx, WeakThis->BallMesh, FName("VfxSocket"));
		});
	}
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTerminalCandleInteraction.AddUObject(this, &ATerminalCandle::Interact);
}

void ATerminalCandle::Interact(const int32 InId)
{
	if (InId != Id) return;
	if (const TObjectPtr<UMaterialInstanceDynamic> MID_Mesh = Mesh->CreateDynamicMaterialInstance(0)) MID_Mesh->SetScalarParameterValue(FName("Color"), 0.666241f);
	if (EnergyVfx)
	{
		EnergyVfx->SetColorParameter(FName("Color"), FLinearColor(1.f, 0.00182f, 0.f));
		EnergyVfx->SetFloatParameter(FName("Lifetime"), 0.f);
		EnergyVfx->Activate(true);
	}
	if (EnergyWireVfx) EnergyWireVfx->Destroy();
	if (const TObjectPtr<UMaterialInstanceDynamic> MID_BallMesh = BallMesh->CreateDynamicMaterialInstance(0)) MID_BallMesh->SetVectorParameterValue(FName("Color"), FLinearColor(0.571f, 0.034f, 0.f));
}
