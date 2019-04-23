//
// Created by vell on 2019/4/20 0020.
//
#pragma once

#include <irrlicht.h>
#include "tesselator.h"

using namespace irr;
using namespace core;
using namespace video;

class TessNode : public scene::ISceneNode {
    aabbox3d<f32> box;
    array<S3DVertex> vertices;
    array<u16> indices;
    SMaterial material;
public:
    TessNode(ISceneNode *parent, scene::ISceneManager *mgr, s32 id,
             const core::vector3df &position, const core::vector3df &rotation,
             const core::vector3df &scale) : ISceneNode(parent, mgr, id, position, rotation,
                                                        scale) {
        material.Wireframe = false;
        material.Lighting = false;
        material.Thickness = 0.f;
        material.MaterialType = EMT_TRANSPARENT_ALPHA_CHANNEL;
//        material.MaterialType = EMT_TRANSPARENT_VERTEX_ALPHA;

//        array<vector3df> keyPoints;
//        keyPoints.push_back(vector3df(0, 0, 0));
//        keyPoints.push_back(vector3df(0, 3, 0));
//        keyPoints.push_back(vector3df(6, 3, 0));
//        keyPoints.push_back(vector3df(5, 9, 0));
//        keyPoints.push_back(vector3df(4, 7, 0));
//        keyPoints.push_back(vector3df(0, -10, 0));
        TESSreal contour[] = {0, 0, 1,
                              6, 0, 1,
                              6, 6, 1,
                              0, 6, 1
        };
        TESSreal contour2[] = {2, 2, 1,
                               4, 2, 1,
                               4, 4, 1,
                               2, 4, 1
        };
        TESStesselator *tess = tessNewTess(nullptr);
        tessAddContour(tess, 3, contour, 12, 4);
        tessAddContour(tess, 3, contour2, 12, 4);
        tessTesselate(tess, TESS_WINDING_ODD, TESS_POLYGONS, 3, 3, nullptr);
        int vertexCount = tessGetVertexCount(tess);
        const TESSreal *tessVertices = tessGetVertices(tess);
        for (int i = 0; i < vertexCount; i++) {
            vertices.push_back(S3DVertex(tessVertices[i * 3],
                                         tessVertices[i * 3 + 1],
                                         tessVertices[i * 3 + 2],
                                         0, 0, -1,
                                         SColor(255, 255, 255, 255), 0, 0));
        }

        int elementCount = tessGetElementCount(tess);
        const int *elements = tessGetElements(tess);
        for (int i = elementCount * 3 - 1; i >= 0; i--) {
            indices.push_back((u16) elements[i]);
        }

        tessDeleteTess(tess);

//        vertices.push_back(S3DVertex(0, 0, 0, 0, 0, -1,
//                                     SColor(255, 0, 255, 255), 0, 0));
//        vertices.push_back(S3DVertex(1, 0, 0, 0, 0, -1,
//                                     SColor(100, 0, 255, 255), 0, 1));
//        vertices.push_back(S3DVertex(0, 1, 0, 0, 0, -1,
//                                     SColor(100, 0, 255, 255), 0.5, 0));
//        vertices.push_back(S3DVertex(1, 1, 0, 0, 0, -1,
//                                     SColor(100, 0, 255, 255), 0.5, 1));
//        vertices.push_back(S3DVertex(0, 2, 0, 0, 0, -1,
//                                     SColor(100, 0, 255, 255), 1, 0));
//        vertices.push_back(S3DVertex(1, 2, 0, 0, 0, -1,
//                                     SColor(255, 0, 255, 255), 1, 1));
//
//        indices.push_back(0);
//        indices.push_back(2);
//        indices.push_back(1);
//
//        indices.push_back(1);
//        indices.push_back(2);
//        indices.push_back(3);
//
//        indices.push_back(3);
//        indices.push_back(2);
//        indices.push_back(4);
//
//        indices.push_back(4);
//        indices.push_back(5);
//        indices.push_back(3);

        box.reset(vertices[0].Pos);
        for (s32 i = 1; i < vertices.size(); ++i)
            box.addInternalPoint(vertices[i].Pos);
    }

    void OnRegisterSceneNode() override {
        if (IsVisible) {
            SceneManager->registerNodeForRendering(this);
        }
        ISceneNode::OnRegisterSceneNode();
    }

    const aabbox3d<f32> &getBoundingBox() const override {
        return box;
    }

    SMaterial &getMaterial(u32 num) override {
        return material;
    }

    u32 getMaterialCount() const override {
        return 1;
    }

    void render() override {
        video::IVideoDriver *driver = SceneManager->getVideoDriver();

        driver->setMaterial(material);
        driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
        driver->drawVertexPrimitiveList(&vertices[0], vertices.size(), &indices[0],
                                        indices.size() / 3, video::EVT_STANDARD,
                                        scene::EPT_TRIANGLES, video::EIT_16BIT);
    }
};