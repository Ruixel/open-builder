#include "chunk_mesh_generation.h"

#include <SFML/System/Clock.hpp>
#include <common/world/chunk.h>
#include <iostream>

#include <common/world/voxel_data.h>

namespace {

const MeshFace FRONT_FACE = {{1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1}, 0.8f};
const MeshFace LEFT_FACE = {{0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1}, 0.6f};
const MeshFace BACK_FACE = {{0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0}, 0.8f};
const MeshFace RIGHT_FACE = {{1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0}, 0.6f};
const MeshFace TOP_FACE = {{1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1}, 1.0f};
const MeshFace BOTTOM_FACE = {{0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1}, 0.4f};

bool makeFace(const VoxelDataManager &voxelData, block_t thisId,
              block_t compareId)
{
    auto &thisBlock = voxelData.getVoxelData(thisId);
    auto &compareBlock = voxelData.getVoxelData(compareId);
    return (compareBlock.id == 0 || compareBlock.id == 4) &&
           thisBlock.id != compareBlock.id;
}

float getAO(const VoxelDataManager& voxelData, block_t topLeftId, 
    block_t topDiagonalId, block_t topRightId) {
    int sum = 0;

    sum += voxelData.getVoxelData(topLeftId).type == VoxelType::Solid;
    sum += voxelData.getVoxelData(topDiagonalId).type == VoxelType::Solid;
    sum += voxelData.getVoxelData(topRightId).type == VoxelType::Solid;

    // Calculate transparency in mesh
    return 1 - (sum / 5.f);
}

inline float ao(int sBlock1, int sBlock2, int sBlock3) {
    return 1 - (sBlock1 + sBlock2 + sBlock3) / 5.f;
}

} // namespace

ChunkMeshCollection makeChunkMesh(const Chunk &chunk,
                                  const VoxelDataManager &voxelData)
{
    sf::Clock clock;
    ChunkMeshCollection meshes(chunk.getPosition());

    // For AO
    std::array<int, 27> surroundingBlocks;
    int counter;

    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                // If it is "not air"
                BlockPosition blockPosition(x, y, z);
                auto voxel = chunk.qGetBlock(blockPosition);
                if (voxel > 0) {

                    auto &voxData = voxelData.getVoxelData(voxel);
                    ChunkMesh *mesh = voxData.type == VoxelType::Solid
                                          ? &meshes.blockMesh
                                          : &meshes.fluidMesh;

                    counter = 0;
                    for (int sy = -1; sy <= 1; sy++) {
                        for (int sx = -1; sx <= 1; sx++) {
                            for (int sz = -1; sz <= 1; sz++) {
                                auto nearBlock = chunk.getBlock(BlockPosition(x + sx, y + sy, z + sz));
                                surroundingBlocks.at(counter++) = voxelData.getVoxelData(nearBlock).type == VoxelType::Solid;
                            }
                        }
                    }

                    // Left block face
                    if (makeFace(voxelData, voxel,
                                 chunk.getBlock({x - 1, y, z}))) {
                        mesh->addFace(LEFT_FACE, blockPosition,
                                      voxData.sideTextureId, { 1, 1, 1, 1 });
                    }

                    // Right chunk face
                    if (makeFace(voxelData, voxel,
                                 chunk.getBlock({x + 1, y, z}))) {
                        mesh->addFace(RIGHT_FACE, blockPosition,
                                      voxData.sideTextureId, { 1, 1, 1, 1 });
                    }

                    // Front chunk face
                    if (makeFace(voxelData, voxel,
                                 chunk.getBlock({x, y, z + 1}))) {
                        mesh->addFace(FRONT_FACE, blockPosition,
                                      voxData.sideTextureId, { 1, 1, 1, 1 });
                    }

                    // Back chunk face
                    if (makeFace(voxelData, voxel,
                                 chunk.getBlock({x, y, z - 1}))) {
                        mesh->addFace(BACK_FACE, blockPosition,
                                      voxData.sideTextureId, { 1, 1, 1, 1 });
                    }

                    // Bottom chunk face
                    if (makeFace(voxelData, voxel,
                                 chunk.getBlock({x, y - 1, z}))) {
                        mesh->addFace(BOTTOM_FACE, blockPosition,
                                      voxData.bottomTextureId, { 1, 1, 1, 1 });
                    }

                    // Top chunk face
                    if (makeFace(voxelData, voxel,
                        chunk.getBlock({ x, y + 1, z }))) {

                        /*auto voxeltL = chunk.getBlock(BlockPosition(x + 1, y + 1, z));
                        auto voxeltR = chunk.getBlock(BlockPosition(x, y + 1, z + 1));
                        auto voxeltD1 = chunk.getBlock(BlockPosition(x + 1, y + 1, z + 1));
                        float aoAlpha1 = getAO(voxelData, voxeltL, voxeltR, voxeltD1);

                        auto voxeltL2 = chunk.getBlock(BlockPosition(x - 1, y + 1, z));
                        auto voxeltR2 = chunk.getBlock(BlockPosition(x, y + 1, z + 1));
                        auto voxeltD2 = chunk.getBlock(BlockPosition(x - 1, y + 1, z + 1));
                        float aoAlpha2 = getAO(voxelData, voxeltL2, voxeltR2, voxeltD2);

                        auto voxeltL3 = chunk.getBlock(BlockPosition(x + 1, y + 1, z));
                        auto voxeltR3 = chunk.getBlock(BlockPosition(x, y + 1, z - 1));
                        auto voxeltD3 = chunk.getBlock(BlockPosition(x + 1, y + 1, z - 1));
                        float aoAlpha3 = getAO(voxelData, voxeltL3, voxeltR3, voxeltD3);

                        auto voxeltL4 = chunk.getBlock(BlockPosition(x - 1, y + 1, z));
                        auto voxeltR4 = chunk.getBlock(BlockPosition(x, y + 1, z - 1));
                        auto voxeltD4 = chunk.getBlock(BlockPosition(x - 1, y + 1, z - 1));
                        float aoAlpha4 = getAO(voxelData, voxeltL4, voxeltR4, voxeltD4);*/

                        float aoAlpha1 = ao(surroundingBlocks[19], surroundingBlocks[20], surroundingBlocks[23]);
                        float aoAlpha2 = ao(surroundingBlocks[21], surroundingBlocks[18], surroundingBlocks[19]);
                        float aoAlpha3 = ao(surroundingBlocks[25], surroundingBlocks[24], surroundingBlocks[21]);
                        float aoAlpha4 = ao(surroundingBlocks[23], surroundingBlocks[26], surroundingBlocks[25]);

                        mesh->addFace(TOP_FACE, blockPosition,
                            voxData.topTextureId, { aoAlpha3, aoAlpha2, aoAlpha1, aoAlpha4 });
                    }
                }
            }
        }
    }
    // std::cout << clock.getElapsedTime().asMilliseconds() << std::endl;

    return meshes;
}
