import { Component } from '@angular/core';
import { NavController, NavParams } from 'ionic-angular';
import  DthKeyData  from '../../data/dthTataSky';

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

  constructor(public navCtrl: NavController, public navParams: NavParams) {}

  ionViewDidLoad() {
    console.log('ionViewDidLoad DthPage');
    //console.log (keyData);
    for(let key of DthKeyData){
        //console.log(key);
    }

  }

  dthKey(keyPressed:string){
    console.log(keyPressed);

    for(let key of DthKeyData){
      if(key.functionname === keyPressed)
      {
        console.log(key);
      }
    }
  }

}
