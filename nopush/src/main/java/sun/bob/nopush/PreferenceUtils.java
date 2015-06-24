package sun.bob.nopush;

import android.content.Context;
import android.content.SharedPreferences;

/**
 * Created by sunkuan on 15/4/18.
 */
public class PreferenceUtils {
    private static PreferenceUtils staticInstance;

    private SharedPreferences sharedPreferences;

    private PreferenceUtils(Context context){
        sharedPreferences = context.getSharedPreferences("daemon_pid",0);
    }

    public static PreferenceUtils getInstance(Context context){
        if(staticInstance == null){
            staticInstance = new PreferenceUtils(context);
        }
        return staticInstance;
    }
    public int getDaemonPid(){
        return Integer.valueOf(sharedPreferences.getString("pid","-1"));
    }

    public void setDaemonPid(String pid){
        if (pid == null || pid == ""){
            pid = "-1";
        }
        sharedPreferences.edit().putString("pid", pid).commit();
    }

    public void setUUID(String uuid){
        sharedPreferences.edit().putString("uuid",uuid).commit();
    }
}
