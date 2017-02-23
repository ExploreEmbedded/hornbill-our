import { Component } from '@angular/core';
import { NavController, AlertController } from 'ionic-angular';
import { RemotePage } from '../remote/remote';
import { TvPage } from '../tv/tv';
import { MusicPage} from '../music/music';
import { DthPage } from '../dth/dth';
import { ConnectPage } from '../connect/connect';
import { BLE } from 'ionic-native';
import { ModalController } from 'ionic-angular';




@Component({
  selector: 'page-home',
  templateUrl: 'home.html'
})
export class HomePage {

  public connectStatus:boolean = false;  //check if the phone is connected to the bluetooth device
  public HORNBILL_DEVICE_ID:string = "";

  public remotesList: { id: number, name: string, icon:string, protocolId:number, keysData:string }[] = [
      { "id": 0, "name": "TV", "icon": "easel" , "protocolId":1, "keysData":"", },
      { "id": 1, "name": "DTH", "icon": "md-square" ,"protocolId":0, "keysData":"",},
      { "id": 2, "name": "Music","icon": "md-musical-notes" , "protocolId":0, "keysData":"",}
  ];

  constructor(public navCtrl: NavController, public alertCtrl: AlertController,private modalCtrl: ModalController,) {
    //this.checkBluetooth();
  }

  //check if bluetooth is enabled on the device.
  checkBluetooth(){
  }

  //ask user to turn on bluetooth
  showConfirm() {
      let confirm = this.alertCtrl.create({
      title: 'Turn ON Bluetooth',
      message: 'Looks like phone bluetooth is disabled, enable it and retry!',
      buttons: [
      {
        text: 'Okay',
        handler: () => {
          console.log('Okay');
        }
      }]
      });
      confirm.present();
  }

  // start scanning this should be moved to connect page
  startScanning(){
    console.log("Scanning Started");
    BLE.scan([],2).subscribe(device => {
        //this.devices.push = device;
        console.log(JSON.stringify(device));
        console.log(device.name);

        //this.devices.push(device);
        //this.stopScanning();
    });
  }




  addRemote(){
      //this.remotesList.push({ "id": 3, "name": "HomeControl", "protocolId":0, "keysData":"",});

  }

  delRemote(){
    //this.remotesList.splice(0,1);
  }


 //handler for connect switch on home page.
  public connectOur(){

    const modal = this.modalCtrl.create(ConnectPage);
    if(this.connectStatus){
      console.log("disconnecting.....");
      this.connectStatus = false;
      // ToDo :disconnect ble later
    }
    else{
      console.log("connecting.....");
      BLE.isEnabled().then(
        ()=>{
            console.log("Bluetooth is enabled on device");
            modal.present();
            modal.onDidDismiss((connectStatusReturned:boolean, deviceId:string) => {
                console.log("deviceid returned "+ deviceId);
                this.HORNBILL_DEVICE_ID = deviceId;
                this.connectStatus = connectStatusReturned;
            });
        },
        ()=>{
            console.log("show message to user");
            this.showConfirm();
        }
      );

    }
    //this.connectStatus =!this.connectStatus ;

  }

  selectDevice(id:number){
    //console.log(id);
    //console.log(this.remotesList[id]);
    switch(id){
      case 0:
        this.navCtrl.push(TvPage, {remotesList:this.remotesList[id], deviceId:this.HORNBILL_DEVICE_ID});
        break;
      case 1:
        this.navCtrl.push(DthPage, this.remotesList[id]);
        break;
      case 2:
        this.navCtrl.push(MusicPage, this.remotesList[id]);
        break;

    }
  }

}
