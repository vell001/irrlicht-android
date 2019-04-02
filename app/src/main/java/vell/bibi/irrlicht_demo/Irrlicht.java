package vell.bibi.irrlicht_demo;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class Irrlicht implements SurfaceHolder.Callback {
    private static final String TAG = Irrlicht.class.getSimpleName();
    private HandlerThread thread;
    private Handler recordHandler;
    private SurfaceView surfaceView;
    private Context context;
    private boolean destroy = false;

    public Irrlicht(Context context, SurfaceView surfaceView) {
        this.context = context;
        this.surfaceView = surfaceView;
        this.surfaceView.getHolder().addCallback(this);
    }

    public void init() {
        // 初始化
        int inited = nativeInitGL(context.getAssets(), surfaceView.getHolder().getSurface(), surfaceView.getWidth(), surfaceView.getHeight());
        Log.i(TAG, "Irrlicht init: " + inited);
        if (inited == 1) {
            thread = new HandlerThread("IrrlichtThread");
            thread.start();
            recordHandler = new Handler(thread.getLooper());
            recordHandler.post(new Runnable() {
                @Override
                public void run() {
                    while (true) {
                        synchronized (surfaceView) {
                            if (destroy) {
                                break;
                            }
                        }
                        try {
                            nativeDrawFrame();
                            Thread.sleep(40);
                        } catch (InterruptedException e) {
                            Log.d(TAG,
                                    "Sleep interrupted: " + e.getMessage());
                        }

                    }
                }
            });
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.i(TAG, "Irrlicht surfaceChanged: " + width + "," + height);
        ((Activity) context).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                init();
            }
        });
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.i(TAG, "Irrlicht surfaceDestroyed");
        synchronized (surfaceView) {
            destroy = true;
        }
    }

    native int nativeInitGL(AssetManager assetManager, Surface surface, int w, int h);

    native void nativeDrawFrame();

    static {
        System.loadLibrary("native-lib");
    }
}
