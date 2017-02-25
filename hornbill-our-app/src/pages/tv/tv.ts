import { Component} from '@angular/core';
import { NavController, NavParams } from 'ionic-angular';
import { BLE } from 'ionic-native';
import {ConnectPage} from '../connect/connect';
import tvKeysData from '../../data/tvVu';



@Component({
  selector: 'page-tv',
  templateUrl: 'tv.html'
})

export class TvPage {

  //this data needs to be fetched from http api.


  private tvData = []; //details about the TV
  public HORNBILL_OUR_ID:string;
  public HORNBILL_OUR_SERVICE:string = "00ff";
  public HORNBILL_OUR_CHARACTERISTICS:string= "ff01";

  public selectedKey = [];
  numPad:boolean = false;

  constructor(public navCtrl: NavController, public navParams: NavParams) {
    this.tvData = navParams.data.remotesList;
    this.HORNBILL_OUR_ID = navParams.data.deviceId;
    //console.log("deviceId "+ deviceId);
  }

  ionViewDidLoad() {
    //console.log(this.tvData);
    //console.log(this.HORNBILL_OUR_ID);
  }

  tvKey(keyPressed:string){
    //console.log(keyPressed);

    for(let key of tvKeysData){
      var newPack = new Uint8Array(4);
      if(key.functionname === keyPressed)
      {
        //console.log(key.function);
        //console.log(parseInt("0x"+ key.function));
        newPack[0] = parseInt("0x"+ key.function)& 255; //first byte
        newPack[1] = (parseInt("0x"+ key.function)>>8)& 255; //second byte
        newPack[2] = (parseInt("0x"+ key.function)>>16)& 255; // third byte
        newPack[3] = 0; //ToDo: can be moved to  a services page and assigned dynamically.
        //console.log(newPack.buffer);
        BLE.write(this.HORNBILL_OUR_ID, this.HORNBILL_OUR_SERVICE, this.HORNBILL_OUR_CHARACTERISTICS, newPack.buffer);
      }
    }
  }



}
