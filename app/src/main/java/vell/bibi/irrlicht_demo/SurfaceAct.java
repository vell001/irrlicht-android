package vell.bibi.irrlicht_demo;

import android.app.Activity;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.SurfaceView;
import android.view.ViewGroup;

public class SurfaceAct extends Activity {
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.surface_act);
        ViewGroup container = findViewById(R.id.container);
        SurfaceView surfaceView = findViewById(R.id.surface_view);
        final Irrlicht irrlicht = new Irrlicht(this, surfaceView);

    }
}
