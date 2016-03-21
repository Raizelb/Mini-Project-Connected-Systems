package com.example.sam.ibmtest;

import android.app.Activity;
import android.app.NotificationManager;
import android.os.Bundle;
import android.support.v4.app.NotificationCompat;
import android.util.Log;

import com.ibm.mobilefirstplatform.clientsdk.android.core.api.BMSClient;
import com.ibm.mobilefirstplatform.clientsdk.android.push.api.MFPPush;
import com.ibm.mobilefirstplatform.clientsdk.android.push.api.MFPPushException;
import com.ibm.mobilefirstplatform.clientsdk.android.push.api.MFPPushResponseListener;
import com.ibm.mobilefirstplatform.clientsdk.android.push.api.MFPPushNotificationListener;
import com.ibm.mobilefirstplatform.clientsdk.android.push.api.MFPSimplePushNotification;

import java.net.MalformedURLException;
import java.util.List;

/**
 * Created by Sam on 19/03/2016.
        */
public class MainActivity extends Activity{

    final String application_route = "http://ucl-sm.eu-gb.mybluemix.net";
    final String application_GUID = "aec6a47d-0241-4553-94df-6df7197dd015";

    private MFPPush push;
    private MFPPushNotificationListener notificationListener;

    @Override
    public void onCreate(Bundle savedInstaceState) {
        super.onCreate(savedInstaceState);
        setContentView(R.layout.activity_main);

        try {
            BMSClient.getInstance().initialize(getApplicationContext(), application_route, application_GUID);
            Log.i("myTag","Initialised");
        }
        catch (MalformedURLException mue) {
            //this.setStatus("Unable to parse Application Route URL\n Please verify you have entered your Application Route and Id correctly", false);
            //buttonText.setClickable(false);
            Log.i("myTag","Initialisation failed");
        }

        //Initialize client Push SDK for Java
        push = MFPPush.getInstance();
        push.initialize(getApplicationContext());

        //Register Android devices
        push.register(new MFPPushResponseListener<String>() {
            @Override
            public void onSuccess(String deviceId) {
                Log.i("myTag", "Success");
            }

            @Override
            public void onFailure(MFPPushException ex) {
                Log.i("myTag", "Failure");
                Log.i("myTag", ex.toString());
            }
        });

        //Handles the notification when it arrives
        notificationListener = new MFPPushNotificationListener() {
            @Override
            public void onReceive (final MFPSimplePushNotification message){
                Log.i("myTag","received notification");
                notifyUser(message.getAlert());
                //notifyUser(message.getPayload().toString());
            }
        };
    }

    @Override
    protected void onResume(){
        super.onResume();
        if (push != null) {
            push.listen(notificationListener);
        }
    }

    private void notifyUser(String message) {
        NotificationCompat.Builder mBuilder =
                new NotificationCompat.Builder(this)
                        .setSmallIcon(R.mipmap.ic_launcher)
                        .setContentTitle("IBM Bluemix")
                        .setContentText(message);
        int mNotificationId = 001;
        // Gets an instance of the NotificationManager service
        NotificationManager mNotifyMgr =
                (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
        // Builds the notification and issues it.
        mNotifyMgr.notify(mNotificationId, mBuilder.build());

    }



}
