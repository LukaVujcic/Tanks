#include <functional>
#include "core/core.h"
#include "core/system.h"
#include "core/game.h"
#include "core/engine.h"
using TileProcessor = std::function<Entity(Registry&, UInt32, UInt32)>;
using TilemapLegend = Map<Char, TileProcessor>;

typedef struct Tilemap
{
	Sequence<Entity> tiles;          // the tiles that have been loaded so far
} Tilemap;

struct TilemapLoadRequest : public AssetLoadRequest<Tilemap>
{
	ViewPtr<TilemapLegend> legend;
};
using namespace dagger;


class TilemapSystem
    : public System
    , public Subscriber<TilemapLoadRequest>
{
    void SpinUp() override;
    void WindDown() override;

    void OnLoadAsset(TilemapLoadRequest request_);

    String SystemName() override;
};
