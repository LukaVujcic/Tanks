#include "tilemap.h"
#include <filesystem>
namespace fs = std::filesystem;
void TilemapSystem::SpinUp()
{
    Engine::Dispatcher().sink<TilemapLoadRequest>().connect<&TilemapSystem::OnLoadAsset>(this);
}

void TilemapSystem::WindDown()
{
    Engine::Dispatcher().sink<TilemapLoadRequest>().disconnect<&TilemapSystem::OnLoadAsset>(this);
}

void TilemapSystem::OnLoadAsset(TilemapLoadRequest request_)
{
    assert(request_.legend != nullptr);
    Tilemap* tilemap = new Tilemap();

    // we'll need these to extract characters and fumble around the map
    Char ch;
    UInt32 x = 0, y = 0;

    // open input file
    FileInputStream input{ request_.path };

    // read everything character by character without skipping whitespace (space, tab, newline)
    while (input >> std::noskipws >> ch) {
        // do work here loading content! the rest of this tutorial will be talking about this!
        if (ch == '\n')
        {
            // newline, go back to the start of the next line
            x = 0; y++;
        }
        else
        {
            // check that our map has this character, debug-fail if not
            assert(request_.legend->contains(ch));

            // call the function mapped under that key with the current coordinates
            // and save that entity into the tiles sequence in our tilemap
            tilemap->tiles.push_back((request_.legend->at(ch))(Engine::Registry(), x, y));
            x++;
        }
    }

    // save the loaded tilemap into shared memory
    Engine::Res<Tilemap>()[request_.path] = tilemap;
}

String TilemapSystem::SystemName()
{
    return "TilemapSystem";
}