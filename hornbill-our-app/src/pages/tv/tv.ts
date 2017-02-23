import { Component} from '@angular/core';
import { NavController, NavParams } from 'ionic-angular';
import { BLE } from 'ionic-native';
import {ConnectPage} from '../connect/connect';


/*
  Generated class for the Tv page.

  See http://ionicframework.com/docs/v2/components/#navigation for more info on
  Ionic pages and navigation.
*/


@Component({
  selector: 'page-tv',
  templateUrl: 'tv.html'
})

export class TvPage {

  //this data needs to be fetched from http api.
  
  public tvKeys:{functionname:string,protocol:string,function:number}[]=[
    {"functionname":"vol+", "protocol":"NEC", "function":0x28d7},
    {"functionname":"vol-", "protocol":"NEC", "function":0x28d9},
    {"functionname":"ch+", "protocol":"NEC", "function":0x28e0},
    {"functionname":"ch-", "protocol":"NEC", "function":0x28e1},
  ];

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
    console.log(this.tvData);
    console.log(this.HORNBILL_OUR_ID);
  }

  tvKey(value:string){
      var newPack = new Uint8Array(2);
      for(let data of this.tvKeys){
          if(data["functionname"] === value){
            console.log("protocol" + data.protocol+ " value "+data.function);
            //data[0] = direction;
            newPack[0] = data.function & 255;
            newPack[1] = (data.function >> 8) & 255;
            console.log(newPack.buffer);
            BLE.write(this.HORNBILL_OUR_ID, this.HORNBILL_OUR_SERVICE, this.HORNBILL_OUR_CHARACTERISTICS, newPack.buffer);
          }
      }
  }



}
