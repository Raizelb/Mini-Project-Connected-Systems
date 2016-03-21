package com.example.sam.ibmtest;

import android.app.Activity;
import android.app.NotificationManager;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.v4.app.NotificationCompat;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.TextView;

import com.ibm.mobilefirstplatform.clientsdk.android.core.api.BMSClient;
import com.ibm.mobilefirstplatform.clientsdk.android.push.api.MFPPush;
import com.ibm.mobilefirstplatform.clientsdk.android.push.api.MFPPushException;
import com.ibm.mobilefirstplatform.clientsdk.android.push.api.MFPPushResponseListener;
import com.ibm.mobilefirstplatform.clientsdk.android.push.api.MFPPushNotificationListener;
import com.ibm.mobilefirstplatform.clientsdk.android.push.api.MFPSimplePushNotification;

import java.net.MalformedURLException;
import java.util.Objects;

/**
 * Created by Sam on 19/03/2016.
        */
public class MainActivity extends Activity{

    final String application_route = "http://connsys-bm.eu-gb.mybluemix.net";
    final String application_GUID = "ca1f0936-6355-42ee-a43c-34a5ce776d6a";
    private static final String SETTING_PREF = "SettingPref";
    private static final String STATE_PREF = "StatePref";
    private static final String LOG_PREF = "LogPref";

    private MFPPush push;
    private MFPPushNotificationListener notificationListener;
    private int mNotificationID = 1;
    private SharedPreferences preferences;
    private MFPPushResponseListener<String> responseListener;
    private TextView logView;
    private String logText;

    @Override
    public void onCreate(Bundle savedInstaceState) {
        super.onCreate(savedInstaceState);
        setContentView(R.layout.activity_main);

        //Init views
        Switch prefSwitch = (Switch) findViewById(R.id.switch1);
        logView = (TextView) findViewById(R.id.textView3);
        Button clearButton = (Button) findViewById(R.id.button);

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
        responseListener = new MFPPushResponseListener<String>() {
            @Override
            public void onSuccess(String deviceId) {
                Log.i("myTag", "Success");
            }

            @Override
            public void onFailure(MFPPushException ex) {
                Log.i("myTag", "Failure");
                Log.i("myTag", ex.toString());
            }
        };

        preferences = this.getSharedPreferences(SETTING_PREF, 0);
        if(preferences.getBoolean(STATE_PREF, true)) {
            push.register(responseListener);
            prefSwitch.setChecked(true);
        } else {
            prefSwitch.setChecked(false);
        }

        //Handles the notification when it arrives
        notificationListener = new MFPPushNotificationListener() {
            @Override
            public void onReceive (final MFPSimplePushNotification message){
                Log.i("myTag", "received notification");
                notifyUser(message.getAlert(), mNotificationID);
                mNotificationID++;
                if(!Objects.equals(logText, "")) {
                    setLog(message.getAlert() + "\n" + logText);
                } else {
                    setLog(message.getAlert());
                }
            }
        };

        //Switch stuff
        prefSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(isChecked) {
                    push.register(responseListener);
                    savePreferences(STATE_PREF, true);
                    //when you set the switch on
                } else {
                    push.unregister(responseListener);
                    savePreferences(STATE_PREF, false);
                    //when you set the switch off
                }
            }
        });

        //Log stuff
        logText = preferences.getString(LOG_PREF, "");
        logView.setText(logText);
        clearButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                setLog("");
            }
        });
    }

    private void savePreferences(String name, boolean value) {
        SharedPreferences.Editor editor = preferences.edit();
        editor.putBoolean(name, value);
        editor.apply();
    }

    private void savePreferences(String name, String value) {
        SharedPreferences.Editor editor = preferences.edit();
        editor.putString(name, value);
        editor.apply();
    }

    private void setLog(String string) {
        logText = string;
        logView.setText(logText);
        savePreferences(LOG_PREF, logText);
    }

    @Override
    protected void onResume(){
        super.onResume();
        if (push != null) {
            push.listen(notificationListener);
        }
    }

    private void notifyUser(String message, int mNotificationId) {
        NotificationCompat.Builder mBuilder =
                new NotificationCompat.Builder(this)
                        .setSmallIcon(R.mipmap.ic_launcher)
                        .setContentTitle("IBM Bluemix")
                        .setContentText(message);
        //int mNotificationId = 001;
        // Gets an instance of the NotificationManager service
        NotificationManager mNotifyMgr =
                (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
        // Builds the notification and issues it.
        mNotifyMgr.notify(mNotificationId, mBuilder.build());

    }

}
