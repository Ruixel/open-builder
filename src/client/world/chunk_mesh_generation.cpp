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

} // namespace

ChunkMeshCollection makeChunkMesh(const Chunk &chunk,
                                  const VoxelDataManager &voxelData)
{
    sf::Clock clock;
    ChunkMeshCollection meshes(chunk.getPosition());

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

                    std::vector<block_t> surroundingBlocks(26);
                    //for 

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

                        auto voxeltL = chunk.getBlock(BlockPosition(x + 1, y + 1, z));
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
                        float aoAlpha4 = getAO(voxelData, voxeltL4, voxeltR4, voxeltD4);

                        mesh->addFace(TOP_FACE, blockPosition,
                            voxData.topTextureId, { aoAlpha3, aoAlpha4, aoAlpha2, aoAlpha1 });
                    }
                }
            }
        }
    }
    // std::cout << clock.getElapsedTime().asMilliseconds() << std::endl;

    return meshes;
}
