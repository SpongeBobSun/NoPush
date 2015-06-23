package sun.bob.nopush;

import android.app.Activity;
import android.app.Application;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import sun.bob.nopush.utils.NotificationUtil;

/**
 * Created by bobsun on 15-6-17.
 */
public class NoPushUtil {
    private Context context;
    private static NoPushUtil staticInstance;
    private String serverAddress;
    private int serverPort;
    private String token;

    private NoPushUtil(Context context, String ip, int port){
        this.context = context;
        this.serverAddress = ip;
        this.serverPort = port;

        //TODO
        //TBD where the service should be pulled up.
        pullUpService();
    }

    public static void initialize(Context context, String ip, int port){
        if (staticInstance == null){
            staticInstance = new NoPushUtil(context, ip, port);
        }
    }

    private void pullUpService(){
        Log.e("PackageName:", context.getPackageName());
        NotificationUtil.getStaticInstance(context);
        Intent intent = new Intent("sun.bob.nopush.pull_up_from_util");
        intent.setPackage(context.getPackageName());
        intent.putExtra("server_addr", this.serverAddress);
        intent.putExtra("server_port",this.serverPort);
        Method myUserHandleMethod = null;
        try {
            Object userManager = context.getSystemService(Context.USER_SERVICE);
            myUserHandleMethod = android.os.Process.class.getMethod("myUserHandle", (Class<?>[]) null);
            Object myUserHandle = myUserHandleMethod.invoke(android.os.Process.class, (Object[]) null);
            Method getSerialNumberForUser = userManager.getClass().getMethod("getSerialNumberForUser", myUserHandle.getClass());
            long userSerial = (Long) getSerialNumberForUser.invoke(userManager, myUserHandle);
            intent.putExtra("pid", (int)userSerial);
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        }

        context.startService(intent);
    }

}
