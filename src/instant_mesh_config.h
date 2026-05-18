/*
    instant_mesh_config.h -- Configuration parameters for Instant Meshes

    This file is part of the implementation of

        Instant Field-Aligned Meshes
        Wenzel Jakob, Daniele Panozzo, Marco Tarini, and Olga Sorkine-Hornung
        In ACM Transactions on Graphics (Proc. SIGGRAPH Asia 2015)

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include "common.h"

struct InstantMeshConfig {
    int rosy = 4;
    int posy = 4;
    Float scale = -1.0f;
    int faceCount = -1;
    int vertexCount = -1;
    Float creaseAngle = -1.0f;
    bool extrinsic = true;
    bool alignToBoundaries = false;
    int smoothIter = 2;
    int knnPoints = 10;
    bool pureQuad = true;
    bool deterministic = false;

    InstantMeshConfig() = default;
};
