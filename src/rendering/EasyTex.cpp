#include "pch.h"
#include "EasyTex.h"

TextureMeta EasyTex::s_metaArray[26] = {
    // IDX  WIDTH       HEIGHT      UV Left,    top,        bottom,     right
    {0,     128 * 2,    128,        0.0f,       0.0f,       0.125f,     0.25f   },  // Ship 2
    {0,     128 * 3,    128,        0.625f,     0.125f,     0.25f,      1.0f    },  // Ship 3
    {0,     128 * 4,    128,        0.5f,       0.25f,      0.375f,     1.0f    },  // Ship 4 - 1
    {0,     128 * 4,    128,        0.25f,      0.0f,       0.125f,     0.75f   },  // Ship 4 - 2
    {0,     128 * 5,    128,        0.0f,       0.125f,     0.25f,      0.625f  },  // Ship 5
    {0,     128,        128,        0.875f,     0.0f,       0.125f,     1.0f    },  // Hit
    {0,     128,        128,        0.75f,      0.0f,       0.125f,     0.875f  },  // No Hit
    {0,     128,        128,        0.625f,     0.375f,     0.5f,       0.75f   },  // Crossair
    {0,     256,        256,        0.25f,      0.25f,      0.5f,       0.5f    },  // Radar Own
    {0,     256,        256,        0.0f,       0.25f,      0.5f,       0.25f   },  // Radar Enemy
    {0,     128,        128,        0.5f,       0.375f,     0.5f,       0.625f  },  // Cursor
    {2,     1920,       1080,       0.0f,       0.0f,       1.0f,       1.0f    },  // Background main
    {1,     1920,       1080,       0.0f,       0.0f,       1.0f,       1.0f    },  // Background game
    {10,    512,        512,        0.0f,       0.0f,       1.0f,       1.0f    },  // Stat: Game Lost
    {9,     512,        512,        0.0f,       0.0f,       1.0f,       1.0f    },  // Stat: Game Win
    {7,     512,        512,        0.0f,       0.0f,       1.0f,       1.0f    },  // Stat: Hit
    {8,     512,        512,        0.0f,       0.0f,       1.0f,       1.0f    },  // Stat: No Hit
    {3,     512,        512,        0.0f,       0.0f,       1.0f,       1.0f    },  // Stat: Place Fleet
    {4,     512,        512,        0.0f,       0.0f,       1.0f,       1.0f    },  // Stat: Place Shot
    {6,     512,        512,        0.0f,       0.0f,       1.0f,       1.0f    },  // Stat: Shot Travel
    {5,     512,        512,        0.0f,       0.0f,       1.0f,       1.0f    },  // Stat: Wait

    {0,     128,        128 * 2,    0.375f,     0.5f,       0.75f,      0.5f    },  // Rotated Ship 2
    {0,     128,        128 * 3,    0.25f,      0.5f,       0.875f,     0.375f  },  // Rotated Ship 3
    {0,     128,        128 * 4,    0.0f,       0.5f,       1.0f,       0.125f  },  // Rotated Ship 4 - 1
    {0,     128,        128 * 4,    0.125f,     0.5f,       1.0f,       0.25f   },  // Rotated Ship 4 - 2
    {0,     128,        128 * 5,    0.875f,     0.375f,     1.0f,       1.0f    },  // Rotated Ship 5
};

TextureQuad EasyTex::textureAt(UINT texture, UINT x, UINT y, UINT8 layer, float scale) {
    // Get Texture meta
    TextureMeta& refMeta = s_metaArray[texture];

    // Normalize start cords
    const float sx = ((x / 1920.0f) * 2.0f) - 1.0f;
    const float sy = 1.0f - ((y / 1080.0f) * 2.0f);

    // Normalize image size
    const float tx = ((refMeta.pixelWidth / 1920.0f) * 2.0f);
    const float ty = ((refMeta.pixelHeight / 1080.0f) * 2.0f);

    // Compute end pos
    const float ex = sx + (scale * tx);
    const float ey = sy - (scale * ty);

    // Gennerate output
    TextureQuad tq;
    // Pos
    tq.top = sy;
    tq.left = sx;
    tq.bottom = ey;
    tq.right = ex;
    // Tex
    tq.uv_top = refMeta.uv_top;
    tq.uv_left = refMeta.uv_left;
    tq.uv_bottom = refMeta.uv_bottom;
    tq.uv_right = refMeta.uv_right;
    // Slot & scale
    tq.texid = refMeta.texIndex;
    tq.layer = layer;

    // Return 
    return tq;
}
