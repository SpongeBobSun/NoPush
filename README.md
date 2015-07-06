#NoPush

** Developing and NOT tested!!! **
##Project Status
Developing and basic functions (message pushing) are available.

Web pages are ugly and primitive.

Notification views on android are ugly and primitive.
##Introduction
NoPush is a message push library for android developers.

Like many other push libraries such as JPush, BaiduPush etc.

But, unlike those libs, push library provide push service with less permissions and it's open source, which give a guarantee of NOT ABUSE and DO NOT DO EVIL. Yes, I said it, DO NOT DO EVIL.

I don't like libs from some "big companies" and with a commercial license and NOT open source. I don't like the feeling of my phone or tablet spied by those "big companies".

I know in anywhere else but China mainland and north Korea, these push libraries are meaningless. They can just use Google's service.

So, there you go. A push lib using ** node js **  as server. Open source. Complete free ( Free software, not free beer).

You can deploy it on your own server and push your message, ads, images to your app users.

##Usage
First, deploy the server on your own server.

You can just use `npm install` and 'node app` to run the server. The server will listen to port 22333 by default.

With the server running, you can visit `http://your.server.address/message` for a message send interface.

It's pretty ugly & savage right now.

Then, import the android side code to your project. Modify AndroidManifest.xml as below.

      <service android:name="sun.bob.nopush.NoPushService"
          android:enabled="true"
          android:process=":NoPushService"
          >
          <intent-filter>
              <category android:name="ANDROID.INTENT.CATEGORY.DEFAULT"/>
              <action android:name="sun.bob.nopush.pull_up_from_util"/>
              <action android:name="sun.bob.nopush.daemon_alive"/>
              <action android:name="sun.bob.nopush.message"/>
          </intent-filter>
          <intent-filter>
              <category android:name="android.intent.category.DEFAULT"/>
              <action android:name="sun.bob.nopush.daemon_pid"/>
          </intent-filter>
      </service>

After this, you can use below code to initialize the library.

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        NoPushUtil.initialize(getApplicationContext(),"your.server.ip.address",22333);
    }

Finally, send a message in your browser and your phone will get a push message.

##How does it work?
Well, it's pretty simple.

When the android library is initialized, it will call a function written in C.

The C code will fork a child process and quit the itself. The child process will initialize socket with given parameters, such as server address, port number etc.

No matter the socket is initialized successfully or not, this process will set a timer and then go to an infinitely loop.

In the infinitely loop, the process will pause and waiting signals arrive. It listen for two signals. One is the timer signal, another is IO ready signal.

When the time signal arrived, it will check the socket is connected or not. Depending on the socket status, it will send a heartbeat or re-initialize the socket.

When the IO ready signal arrived, it will perform corresponding IO action. If a message from server arrived, it will call `am` and send a broadcast, or start a service, more precisely. And then....let's back to the Java side.

Java service will take the broadcast and parse the message. According to the message content, it will send a notification.

BTW, when I'm talking about signal, I mean Linux signals.

That's all. Pretty lightweight and it just get the job DONE. No more permissions required. No spy code. No James Bond in our devices.
## About the name
Hey, why NoPush? Is it a push library or not?

Just a combination of Nodejs and Push. And I think it's a funny name. Sue me.