package sun.bob.nopush.utils;

import android.app.ActivityManager;
import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;
import android.widget.RemoteViews;

import org.json.JSONException;
import org.json.JSONObject;

import sun.bob.nopush.R;

/**
 * Created by bobsun on 15-6-23.
 */
public class NotificationUtil {
    private Context appContext;
    private static NotificationUtil staticInstance;
    NotificationManager notificationManager;
    Notification textNotification;
    private NotificationUtil(Context context){
        appContext = context;
        notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
    }

    public static NotificationUtil getStaticInstance(Context context){
        if (staticInstance == null){
            staticInstance = new NotificationUtil(context);
        }
        return staticInstance;
    }

    public void sendTextNotification(String raw){
        String text = null;
        try {
            text = parse(raw);
        } catch (JSONException e) {
            text = raw;
        }
        textNotification = new Notification();
        textNotification.icon = R.drawable.ic_launcher;
        textNotification.tickerText = text;
        textNotification.when = System.currentTimeMillis();
        textNotification.contentView = new RemoteViews(appContext.getPackageName(),R.layout.notification_text_only);
        textNotification.contentView.setCharSequence(R.id.id_text_view_text_only, "setText", text);
        textNotification.defaults = Notification.DEFAULT_ALL;
        notificationManager.notify("text_notification",100,textNotification);
    }

    private String parse(String raw) throws JSONException {
        String ret = "";
        JSONObject json = new JSONObject(raw);
        String notification = json.getString("notification");
        if (notification != null){
            return notification;
        }
        String message = json.getString("message");
        if (message != null){
            return message;
        }
        String icon = json.getString("icon");
        return ret;
    }


}
