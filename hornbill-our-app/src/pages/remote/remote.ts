import { Component } from '@angular/core';
import { NavController, NavParams } from 'ionic-angular';

/*
  Generated class for the Remote page.

  See http://ionicframework.com/docs/v2/components/#navigation for more info on
  Ionic pages and navigation.
*/
@Component({
  selector: 'page-remote',
  templateUrl: 'remote.html'
})
export class RemotePage {

  selectedDevice = []; // Selected Remote device configuration
  testNumber:number = 1;

  constructor(public navCtrl: NavController, public navParams: NavParams) {
    this.selectedDevice = navParams.data;
    //console.log(this.selectedDevice);
  }

  ionViewDidLoad() {
    console.log('ionViewDidLoad RemotePage');
  }



}
