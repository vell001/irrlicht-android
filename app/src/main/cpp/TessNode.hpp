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

        array<vector3df> testLine;
        testLine.push_back(vector3df(0, 0, 0));
        testLine.push_back(vector3df(6, 0, 0));
        testLine.push_back(vector3df(6, 6, 0));
        testLine.push_back(vector3df(9, 9, 0));
        testLine.push_back(vector3df(10, 15, 0));
        testLine.push_back(vector3df(15, 10, 0));

        array<vector3df> leftLine;
        array<vector3df> rightLine;
        offsetLine(testLine, leftLine, rightLine, 2);
        array<TESSreal> contour;
        for (int i = 0; i < leftLine.size(); i++) {
            contour.push_back(leftLine[i].X);
            contour.push_back(leftLine[i].Y);
            contour.push_back(leftLine[i].Z);

            __android_log_print(ANDROID_LOG_ERROR, "TessNode", "left-%d: (%f,%f,%f)", i,
                                leftLine[i].X, leftLine[i].Y, leftLine[i].Z);
        }

        for (int i = rightLine.size() - 1; i >= 0; i--) {
            contour.push_back(rightLine[i].X);
            contour.push_back(rightLine[i].Y);
            contour.push_back(rightLine[i].Z);

            __android_log_print(ANDROID_LOG_ERROR, "TessNode", "right-%d: (%f,%f,%f)", i,
                                rightLine[i].X, rightLine[i].Y, rightLine[i].Z);
        }

        TESStesselator *tess = tessNewTess(nullptr);
        tessAddContour(tess, 3, &contour[0], 12, leftLine.size() + rightLine.size());
//        tessAddContour(tess, 3, &rightContour[0], 12, rightContour.size());
        tessTesselate(tess, TESS_WINDING_ODD, TESS_POLYGONS, 3, 3, nullptr);
        int vertexCount = tessGetVertexCount(tess);
        const TESSreal *tessVertices = tessGetVertices(tess);
        for (int i = 0; i < vertexCount; i++) {
            vertices.push_back(S3DVertex(tessVertices[i * 3],
                                         tessVertices[i * 3 + 1],
                                         tessVertices[i * 3 + 2],
                                         0, 0, -1,
                                         SColor(150, 0, 0, 200), 0, 0));
        }

        int elementCount = tessGetElementCount(tess);
        const int *elements = tessGetElements(tess);
//        for (int i = elementCount * 3 - 1; i >= 0; i--) {
//            indices.push_back((u16) elements[i]);
//        }
        for (int i = 0; i < elementCount * 3; i++) {
            indices.push_back((u16) elements[i]);
        }

        tessDeleteTess(tess);

        box.reset(vertices[0].Pos);
        for (s32 i = 1; i < vertices.size(); ++i)
            box.addInternalPoint(vertices[i].Pos);
    }

    void offsetLine(const array<vector3df> &line, array<vector3df> &leftLine,
                    array<vector3df> &rightLine, f32 thickness = 1.0f) {

        for (int i = 1; i < line.size() - 1; i++) {
            const vector3df &p1 = line[i - 1];
            const vector3df &c = line[i];
            const vector3df &p2 = line[i + 1];
            vector3df v1 = c - p1;
            vector3df v2 = p2 - c;
            vector3df n1 = (v1.normalize() * thickness);
            n1.rotateXYBy(90);
            vector3df n2 = (v2.normalize() * thickness);
            n2.rotateXYBy(90);

            vector3df p1a = p1 + n1;
            vector3df p1b = p1 - n1;

            vector3df c1a = c + n1;
            vector3df c1b = c - n1;
            vector3df c2a = c + n2;
            vector3df c2b = c - n2;

            vector3df p2a = p2 + n2;
            vector3df p2b = p2 - n2;

            if (i == 1) {
                leftLine.push_back(p1a);
                rightLine.push_back(p1b);
            }


            vector3df ca;
            intersection(p1a, c1a, p2a, c2a, ca);

            vector3df cb;
            intersection(p1b, c1b, p2b, c2b, cb);

            leftLine.push_back(ca);
            rightLine.push_back(cb);

            if (i == line.size() - 2) {
                leftLine.push_back(p2a);
                rightLine.push_back(p2b);
            }
        }
    }

    bool intersection(const vector3df &lineAStart, const vector3df &lineAEnd,
                      const vector3df &lineBStart, const vector3df &lineBEnd,
                      vector3df &intersectPos) {
        vector3df da = lineAEnd - lineAStart;
        vector3df db = lineBEnd - lineBStart;
        vector3df dc = lineBStart - lineAStart;
        if (dc.dotProduct(da.crossProduct(db)) != 0.0f) {
            // 不共面
            return false;
        }
        f32 s = dc.crossProduct(db).dotProduct(da.crossProduct(db)) /
                da.crossProduct(db).getLengthSQ();
//        if (s >= 0 && s <= 1.0) {
//
//            return true;
//        }
        intersectPos = lineAStart + da * vector3df(s, s, s);
        return true;
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