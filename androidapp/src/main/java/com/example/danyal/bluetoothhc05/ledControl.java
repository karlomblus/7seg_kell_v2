package com.example.danyal.bluetoothhc05;

import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.graphics.Color;
import android.os.AsyncTask;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.UUID;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class ledControl extends AppCompatActivity {

    Button btn_wrong, btn_settime, btn_loe, btnDis, btn_m0, btn_m1, btn_m2, btn_m3, btn_m4, btn_m5;
    //, btn_send;
    //EditText intxt;
    String address = null;
    TextView lumn, txt_log;
    private ProgressDialog progress;
    BluetoothAdapter myBluetooth = null;
    BluetoothSocket btSocket = null;
    private boolean isBtConnected = false;
    static final UUID myUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Intent newint = getIntent();
        address = newint.getStringExtra(DeviceList.EXTRA_ADDRESS);

        setContentView(R.layout.activity_led_control);

        btn_wrong = (Button) findViewById(R.id.btn_wrong);
        btn_settime = (Button) findViewById(R.id.btn_settime);
        btn_loe = (Button) findViewById(R.id.btn_loe);
        btnDis = (Button) findViewById(R.id.button4);

        btn_m0 = (Button) findViewById(R.id.btn_m0);
        btn_m1 = (Button) findViewById(R.id.btn_m1);
        btn_m2 = (Button) findViewById(R.id.btn_m2);
        btn_m3 = (Button) findViewById(R.id.btn_m3);
        btn_m4 = (Button) findViewById(R.id.btn_m4);
        btn_m5 = (Button) findViewById(R.id.btn_m5);


        //btn_send = (Button) findViewById(R.id.btn_send);
        //intxt = (EditText) findViewById(R.id.in_nr);

        lumn = (TextView) findViewById(R.id.textView2);
        txt_log = (TextView) findViewById(R.id.textLog);

        txt_log.setMovementMethod(new ScrollingMovementMethod());
        new ConnectBT().execute();

/*
        btn_send.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String innr = intxt.getText().toString();
                intxt.setText("");
                //String tosend = new StringBuilder(innr.substring(0,Math.min(6,innr.length()))).reverse().toString();


                sendSignal(innr.substring(0, Math.min(6, innr.length())) + "b\r\n");
            }
        });
*/
        btn_m0.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendSignal("0b\r\n");
            }
        });
        btn_m1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendSignal("1b\r\n");
            }
        });
        btn_m2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendSignal("2b\r\n");
            }
        });
        btn_m3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendSignal("3b\r\n");
            }
        });
        btn_m4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendSignal("4b\r\n");
            }
        });
        btn_m5.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendSignal("5b\r\n");
            }
        });


        // paneme demoks kella valeks
        btn_wrong.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendSignal("00h11m22s\r\n");
            }
        });

        btn_settime.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Date currentTime = Calendar.getInstance().getTime();
                SimpleDateFormat sdfh = new SimpleDateFormat("HH");
                SimpleDateFormat sdfm = new SimpleDateFormat("mm");
                SimpleDateFormat sdfs = new SimpleDateFormat("ss");
                sendSignal(sdfh.format(new Date()) + "h" + sdfm.format(new Date()) + "m" + sdfs.format(new Date()) + "s\r\n");
            }
        });


        btn_loe.setText("Loe");
        btn_loe.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendSignal("u\r\n"); // küsime kõigi nuppude staatust
                do_lugemine();
            }
        });

        btnDis.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Disconnect();
            }
        });
    }


    private void sendSignal(String number) {
        Log.i("karl", "Saadame signaali: " + number);
        if (btSocket != null) {
            try {
                btSocket.getOutputStream().write(number.toString().getBytes());
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                do_lugemine();
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                do_lugemine();
            } catch (IOException e) {
                msg("Error");
            }
        }
    }

    private void Disconnect() {
        if (btSocket != null) {
            try {
                btSocket.close();

            } catch (IOException e) {
                msg("Error");
            }
        }

        finish();
    }

    private void msg(String s) {
        Log.i("karl", "msg: " + s);
        Toast.makeText(getApplicationContext(), s, Toast.LENGTH_LONG).show();
    }

    private class ConnectBT extends AsyncTask<Void, Void, Void> {

        private boolean ConnectSuccess = true;
        @Override
        protected void onPreExecute() {
            progress = ProgressDialog.show(ledControl.this, "Connecting...", "Please Wait!!!");
        }

        @Override
        protected Void doInBackground(Void... devices) {
            try {
                if (btSocket == null || !isBtConnected) {
                    myBluetooth = BluetoothAdapter.getDefaultAdapter();
                    BluetoothDevice dispositivo = myBluetooth.getRemoteDevice(address);
                    btSocket = dispositivo.createInsecureRfcommSocketToServiceRecord(myUUID);
                    BluetoothAdapter.getDefaultAdapter().cancelDiscovery();
                    btSocket.connect();
                }
            } catch (IOException e) {
                ConnectSuccess = false;
            }

            return null;
        }

        @Override
        protected void onPostExecute(Void result) {
            super.onPostExecute(result);

            if (!ConnectSuccess) {
                msg("Connection Failed. Is it a SPP Bluetooth? Try again.");
                finish();
            } else {
                msg("Connected");
                isBtConnected = true;
            }

            progress.dismiss();
        }

    }
    private void do_lugemine() {
        {

            new Thread(new Runnable() {
                public void run() {
                    // a potentially time consuming task


                    InputStream tmpIn = null;
                    String mybuff = "";

                    try {
                        tmpIn = btSocket.getInputStream();
                        DataInputStream in = new DataInputStream(tmpIn);

                        while (in.available() > 0) {

                            byte[] messageByte = new byte[1000];

                            int bytesRead = in.read(messageByte);
                            final String buff = new String(messageByte, 0, bytesRead);
                            mybuff += buff; // korjan omale bufrit, et vastust parsida
                            while (mybuff.contains("\n")) {
                                String[] parts = mybuff.split("\n", 2);
                                mybuff = parts[1]; // ülejäänu jääb bufrisse
                                String saime = parts[0];
                                Pattern pattern = Pattern.compile("m([0-9]):(.)");
                                Matcher matcher = pattern.matcher(saime);
                                if (matcher.find()) {
                                    int kes = Integer.parseInt(matcher.group(1));
                                    final int mis = Integer.parseInt(matcher.group(2));
                                    Log.i("karl", "saime tagasi, kes (" + kes + "): mis: " + mis);

                                    final Button Bkes;
                                    if (kes == 0) Bkes = btn_m0;
                                    else if (kes == 1) Bkes = btn_m1;
                                    else if (kes == 2) Bkes = btn_m2;
                                    else if (kes == 3) Bkes = btn_m3;
                                    else if (kes == 4) Bkes = btn_m4;
                                    else if (kes == 5) Bkes = btn_m5;
                                    else Bkes = null;
                                    if (Bkes != null) {
                                        Bkes.post(new Runnable() {
                                            public void run() {
                                                if (mis == 0) {
                                                    Bkes.setBackgroundColor(Color.RED);
                                                    Bkes.setTextColor(Color.WHITE);
                                                } else {
                                                    Bkes.setBackgroundColor(Color.GREEN);
                                                    Bkes.setTextColor(Color.BLACK);
                                                }
                                            }
                                        });
                                    }
                                }

                            }

                            txt_log.post(new Runnable() {
                                public void run() {
                                    txt_log.append(buff);
                                    //txt_log.scrollTo(0,txt_log.getBottom());
                                }
                            });
                            Log.i("karl", "Readback (" + bytesRead + "): " + buff);
                        }

                        Log.i("karl", "DIS closed");
                        txt_log.post(new Runnable() {
                            public void run() {
                                txt_log.append("dis closed");
                                //txt_log.scrollTo(0,txt_log.getBottom());
                            }
                        });

                    } catch (IOException e) {
                        Log.e("karl", "Error occurred when creating input stream", e);
                    }


                }
            }).start();


        }

    }
}
