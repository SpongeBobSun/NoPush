package sun.bob.nopush;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.*;
import android.os.Process;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.EOFException;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.CharBuffer;


/**
 * Created by bobsun on 15-6-17.
 */
public class NoPushService extends Service {

    public final int SIGCHECKDEAMON = 50;

    public final ServiceBinder binder = new ServiceBinder();

    public static boolean daemonAlive;

    static {
        System.loadLibrary("nopush");
    }

    public native String entry(String addr, int port, int pid);

    public NoPushService(){

    }

    @Override
    public void onCreate(){
        super.onCreate();
    }

    @Override
    public int onStartCommand(Intent intent, int flags,int startId) {
        if (intent.getAction()!= null && intent.getAction().equalsIgnoreCase("sun.bob.nopush.pull_up_from_util")) {
            if (!checkDaemon())
                entry(intent.getStringExtra("server_addr"), intent.getIntExtra("server_port", 22333),intent.getIntExtra("pid",0));
        }

        if (intent.getAction()!= null && intent.getAction().equalsIgnoreCase("sun.bob.nopush.daemon_alive")){
            daemonAlive = true;
        }
        if (intent.getAction()!= null && intent.getAction().equalsIgnoreCase("sun.bob.nopush.daemon_pid")){
            String pid = intent.getStringExtra("pid");
            if (pid == null || pid == ""){
                daemonAlive = false;
                return START_REDELIVER_INTENT;
            }
            PreferenceUtils.getInstance(getApplicationContext()).setDaemonPid(pid);
            Log.e("pid====================", "" + pid);
        }
        if (intent.getAction()!= null && intent.getAction().equalsIgnoreCase("sun.bob.nopush.message")){
            Log.e("Receive data",intent.getStringExtra("rawData"));
        }
        return START_REDELIVER_INTENT;
    }

    public boolean checkDaemonAlive(){
        int pid = PreferenceUtils.getInstance(getApplicationContext()).getDaemonPid();
        if (pid < 0){
            daemonAlive = false;
            return false;
        }
        Process.sendSignal(pid,SIGCHECKDEAMON);
        daemonAlive = false;
        return true;
    }

    public boolean checkDaemon(){
        int pid = PreferenceUtils.getInstance(getApplicationContext()).getDaemonPid();
        Log.e("Saved pid:",""+pid);
        if (pid < 0){
            daemonAlive = false;
            return false;
        }
        try {
            char[] rawBuffer = new char[1024];
            FileReader fileReader = new FileReader("/proc/"+pid+"/cmdline");
            fileReader.read(rawBuffer);
            String buffer = String.copyValueOf(rawBuffer);
            buffer = buffer.substring(0,buffer.indexOf('\0'));
            if (buffer.endsWith(":NoPushService")) {
                Log.e("NoPushService","Daemon alive. Thank god.");
                daemonAlive = true;
                return true;
            }
            Log.e("Push daemon","is invalid");
        } catch (FileNotFoundException e) {
            Log.e("Push daemon process"," not found in /proc");
        } catch (EOFException e) {
            Log.e("EOF","Exception");
        } catch (IOException e) {
            Log.e("IO","Exception");
        }
        daemonAlive = false;
        return false;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return binder;
    }

    class ServiceBinder extends Binder {
        public Service getService(){
            return NoPushService.this;
        }
    }
}
