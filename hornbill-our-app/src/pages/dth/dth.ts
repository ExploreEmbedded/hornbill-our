import { Component } from '@angular/core';
import { NavController, NavParams } from 'ionic-angular';
import  DthKeyData  from '../../data/dthTataSky';
import { BLE } from 'ionic-native';


/*
  Generated class for the Dth page.

  See http://ionicframework.com/docs/v2/components/#navigation for more info on
  Ionic pages and navigation.
*/
@Component({
  selector: 'page-dth',
  templateUrl: 'dth.html'
})
export class DthPage {

  //ToDo: Move to remoteServices
  public HORNBILL_OUR_ID:string;
  public HORNBILL_OUR_SERVICE:string = "00ff";
  public HORNBILL_OUR_CHARACTERISTICS:string= "ff01";
  deviceData = [];

  constructor(public navCtrl: NavController, public navParams: NavParams) {
    this.HORNBILL_OUR_ID = navParams.data.deviceId;
    this.deviceData = navParams.data.remotesList;
  }

  ionViewDidLoad() {
  //  console.log('ionViewDidLoad DthPage');
  }

  dthKey(keyPressed:string){
    console.log(keyPressed);

    for(let key of DthKeyData){
      var newPack = new Uint8Array(4);
      if(key.functionname === keyPressed)
      {
        console.log(key.function);
        console.log(parseInt("0x"+ key.function));
        newPack[0] = parseInt("0x"+ key.function)& 255; //first byte
        newPack[1] = (parseInt("0x"+ key.function)>>8)& 255; //second byte
        newPack[2] = (parseInt("0x"+ key.function)>>16)& 255; // third byte
        newPack[3] = 1; //ToDo: can be moved to  a services page and assigned dynamically.
      //  console.log(newPack.buffer);
        BLE.write(this.HORNBILL_OUR_ID, this.HORNBILL_OUR_SERVICE, this.HORNBILL_OUR_CHARACTERISTICS, newPack.buffer);
      }
    }
  }

}
