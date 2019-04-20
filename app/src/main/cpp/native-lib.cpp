#include <jni.h>
#include <string>
#include <irrlicht.h>
#include <android/log.h>
#include "android_tools.h"
#include "android/window.h"
#include <android/native_window_jni.h>
#include <android/looper.h>
#include "SplineNode.hpp"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

extern "C" JNIEXPORT jstring JNICALL
Java_vell_bibi_irrlicht_1demo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

/*  不使用nativeActivity  */

IrrlichtDevice *device = nullptr;
IVideoDriver *driver = nullptr;

ISceneManager *smgr = nullptr;
IGUIEnvironment *guienv = nullptr;
ANativeWindow *window = nullptr;
int width = 0;
int height = 0;
static android_app app;

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

extern "C"
JNIEXPORT jint JNICALL
Java_vell_bibi_irrlicht_1demo_Irrlicht_nativeInitGL(JNIEnv *env, jobject instance,
                                                    jobject assetManager, jobject surface,
                                                    jint w, jint h) {

    window = ANativeWindow_fromSurface(env, surface);
//    ALooper *looper = ALooper_forThread(); // get looper for this thread
//    ALooper_acquire(looper); // add reference to keep object alive
//    ALooper* looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
//    app.looper = looper;
    if (window) {
        app.window = window;
        SIrrlichtCreationParameters p;
        p.DriverType = video::EDT_OGLES2;
        p.WindowSize = dimension2d<u32>(static_cast<const u32 &>(w), static_cast<const u32 &>(h));
        p.Bits = 24;
        p.ZBufferBits = 16;
        p.Fullscreen = false;
        p.EventReceiver = 0;
        p.PrivateData = (void *) &app;
        ANativeActivity nativeActivity;
        nativeActivity.assetManager = AAssetManager_fromJava(env, assetManager);
        app.activity = &nativeActivity;
        device = createDeviceEx(p);
        if (device) {
            driver = device->getVideoDriver();
            if (driver) {
                smgr = device->getSceneManager();
                guienv = device->getGUIEnvironment();
                IFileSystem *fs = device->getFileSystem();

                IGUISkin *skin = guienv->getSkin();
                video::SColor color = skin->getColor(EGDC_WINDOW);
                color.setAlpha(150);
                skin->setColor(EGDC_WINDOW, color);

                /*
                    Add a 3d model. Note that you might need to add light when using other models.
                    A copy of the model and it's textures must be inside the assets folder to be installed to Android.
                    In this example we do copy it to the assets folder in the Makefile jni/Android.mk
                */
                stringc mediaPath = "media/";

                // The Android assets file-system does not know which sub-directories it has (blame google).
                // So we have to add all sub-directories in assets manually. Otherwise we could still open the files,
                // but existFile checks will fail (which are for example needed by getFont).
                for (u32 i = 0; i < fs->getFileArchiveCount(); ++i) {
                    IFileArchive *archive = fs->getFileArchive(i);
                    if (archive->getType() == EFAT_ANDROID_ASSET) {
                        archive->addDirectoryToFileList(mediaPath);
                        break;
                    }
                }

                IAnimatedMesh *mesh = smgr->getMesh(mediaPath + "arrow.dae");
                if (!mesh) {
                    device->closeDevice();
                    device->drop();
                    return 0;
                }

                S3DVertex *v = static_cast<S3DVertex *>(mesh->getMeshBuffer(0)->getVertices());

                for (int i = 0; i < mesh->getMeshBuffer(0)->getVertexCount(); i++) {
                    v[i].Color.set(100, 255, 255, 255);
                }


                IAnimatedMeshSceneNode *node = smgr->addAnimatedMeshSceneNode(mesh);
                if (node) {
                    node->setMaterialFlag(EMF_LIGHTING, false);
                    node->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
                    node->setMD2Animation(scene::EMAT_STAND);
//                    node->addAnimator(smgr->createFlyCircleAnimator(vector3df(0, 0, 0), 1.0));

                    core::array<core::vector3df> points;
                    points.push_back(core::vector3df(0, 0, 0));
                    points.push_back(core::vector3df(3, 0, 0));
                    points.push_back(core::vector3df(3, 3, 0));
                    points.push_back(core::vector3df(3, 3, 3));

                    node->addAnimator(smgr->createFollowSplineAnimator(0, points));
                    node->addAnimator(
                            smgr->createRotationAnimator(vector3df(0.3f, 0.3f, 0.3f)));
                    node->setScale(vector3df(1.3f, 1.0f, 1.0f));
                }

//                ISceneNode *splineNode = new SplineNode(smgr->getRootSceneNode(), smgr, 666, vector3df(0, 0, 0),
//                                                        vector3df(0, 0, 0), vector3df(0, 0, 0));
                ISceneNode *splineNode = new SplineNode(smgr->getRootSceneNode(), smgr, -1,
                                                        vector3df(0, 0, 0),
                                                        vector3df(0, 0, 0),
                                                        vector3df(2.0f, 2.0f, 2.0f));
//                driver->setTextureCreationFlag(ETCF_ALWAYS_32_BIT);
                ITexture *texture = driver->getTexture(mediaPath + "earth.jpg");
//                driver->makeColorKeyTexture(texture,position2di(0,0));
                splineNode->setMaterialTexture(0, texture);
//                texture->drop();
                scene::ISceneNodeAnimator *anim =
                        smgr->createRotationAnimator(core::vector3df(0.8f, 0.8f, 0.8f));

                if (anim) {
//                    splineNode->addAnimator(anim);

                    /*
                    I'm done referring to anim, so must
                    irr::IReferenceCounted::drop() this reference now because it
                    was produced by a createFoo() function. As I shouldn't refer to
                    it again, ensure that I can't by setting to 0.
                    */
                    anim->drop();
                    anim = 0;
                }

//                smgr->addLightSceneNode(0, vector3df(1, 1, 1));

                /*
                To look at the mesh, we place a camera.
                */
                smgr->addCameraSceneNode(0, vector3df(0, 0, -6), vector3df(0, 0, 0));
                return 1;
            }
        }
    }

    return 0;
}
extern "C"
JNIEXPORT void JNICALL
Java_vell_bibi_irrlicht_1demo_Irrlicht_nativeDrawFrame(JNIEnv *env, jobject instance) {
    if (device && device->run()) {
        device->getVideoDriver()->beginScene(true, true, SColor(0, 0, 0, 0));
        device->getSceneManager()->drawAll();
        device->getGUIEnvironment()->drawAll();
        device->getVideoDriver()->endScene();
        device->yield(); // probably nicer to the battery
        __android_log_print(ANDROID_LOG_ERROR, "test", "nativeDrawFrame");
    }
}