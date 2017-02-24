import { Component } from '@angular/core';
import { NavController, NavParams } from 'ionic-angular';
import musicKeyData from '../../data/musicIntex';

@Component({
  selector: 'page-music',
  templateUrl: 'music.html'
})
export class MusicPage {

  constructor(public navCtrl: NavController, public navParams: NavParams) {}

  ionViewDidLoad() {

  }

  musicKey(keyPressed:string){
    for(let key of musicKeyData){
      if(key.functionname === keyPressed)
      {
        console.log(keyPressed +"\t");
        console.log(key.function);
        //console.log(parseInt("0x"+ key.function));
      }
    }
  }


}
