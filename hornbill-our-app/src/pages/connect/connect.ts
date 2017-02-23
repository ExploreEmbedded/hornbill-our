import { Component } from '@angular/core';
import { NavController, NavParams } from 'ionic-angular';
import { ViewController } from "ionic-angular";
import { BLE } from 'ionic-native';



/*
  Generated class for the Connect page.

  See http://ionicframework.com/docs/v2/components/#navigation for more info on
  Ionic pages and navigation.
*/
@Component({
  selector: 'page-connect',
  templateUrl: 'connect.html'
})
export class ConnectPage {

  devices = [];

  public HORNBILL_OUR_ID: string;


  constructor(public navCtrl: NavController, public navParams: NavParams, private viewCtrl: ViewController) {
    //get the list of devices so that we can connect
    this.startScanning();
  }

  ionViewDidLoad() {
    console.log('ionViewDidLoad ConnectPage');
  }

  startScanning(){
    console.log("Scanning Started");
    BLE.scan([],2).subscribe(device => {
        //this.devices.push = device;
        console.log(JSON.stringify(device));
        console.log(device.name);
        this.devices.push(device);
        //this.stopScanning();
    });
  }

  selectDevice(deviceId:string){
    console.log("trying to connect");
    BLE.connect(deviceId).subscribe(peripheralData => {
        console.log(peripheralData);
        //this.deviceServices = peripheralData.services;
      //  console.log(this.deviceServices);
      this.HORNBILL_OUR_ID = deviceId;
      this.onClose(true);
      },
      peripheralData => {
        console.log('disconnected');
        this.onClose(false);
      });
  }


  onClose(connectedStatusReturned = true) {
    this.viewCtrl.dismiss(connectedStatusReturned, this.HORNBILL_OUR_ID);
  }

}
