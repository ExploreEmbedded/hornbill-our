import { NgModule, ErrorHandler } from '@angular/core';
import { IonicApp, IonicModule, IonicErrorHandler } from 'ionic-angular';
import { MyApp } from './app.component';
import { HomePage } from '../pages/home/home';
import { TvPage } from '../pages/tv/tv';
import { DthPage} from '../pages/dth/dth';
import { MusicPage } from '../pages/music/music';
import { ConnectPage } from '../pages/connect/connect';
import { HornbillBlueService } from './hornbill-blue.service';


@NgModule({
  declarations: [
    MyApp,
    HomePage,
    DthPage,
    MusicPage,
    TvPage,
    ConnectPage
  ],
  imports: [
    IonicModule.forRoot(MyApp)
  ],
  bootstrap: [IonicApp],
  entryComponents: [
    MyApp,
    HomePage,
    DthPage,
    MusicPage,
    TvPage,
    ConnectPage
  ],
  providers: [{provide: ErrorHandler, useClass: IonicErrorHandler}, HornbillBlueService]
})
export class AppModule {}
