import threading
import datetime
import pyrogue as pr

from sqlalchemy import Integer, DateTime, Text
from sqlalchemy.orm import Mapped, mapped_column

class Run(ldmx_tdaq.SqliteDatabase.SqliteBase):
    __tablename__ = 'runs'

    id: Mapped[int] = mapped_column(Integer, primary_key=True, autoincrement=True)
    start_time: Mapped[datetime.datetime] = mapped_column(DateTime)
    end_time: Mapped[datetime.datetime] = mapped_column(DateTime)
    config: Mapped[str] = mapped_column(Text)

class S30xlRunControl(pr.RunControl):
    def __init__(self, database, apx, **kwargs):
        states = {0: 'Stopped', 1: 'Running'}
        rates = {0: 'InternalTrig', 1: '1 Hz'}
        super().__init__(name='RunControl', rates=rates, states=states, **kwargs)

        
    def __prestart(self):
        fcHub = self.apx.FcHub
        
        print('Capturing configuration')
        yaml_config = self.root.GetYamlConfig(arg=True)

        # Don't create run entry in db until system status checks have been completed
        print('Checking link status')
        bad = False
        lclsTimingLink = fcHub.Lcls2TimingRx.TimingFrameRx.RxLinkUp.get()
        if not lclsTimingLink:
            print('LCLS Timing not linked')
            bad = True
        
        fcHubAppLink = fcHub.FcSenderLane[0].LinkUp.get()
        if not fcHubAppLink:
            print('Bad FcHub->S30xlApp FC Link')
            bad = True
            
        #fcHubzccmLink = fcHub.FcSenderLane[1].LinkUp.get()

        appFcHubLink = self.apx.S30xlAppCore.FcReceiver.PgpFcLane.LinkUp.get()
        if not appFcHubLink:
            print('Bad S30xlApp FC Link to Hub')
            bad = True
        
        
        print('Creating Run in DB')
        with self.database.SessionFactory() as session:
            new_run = Run(config = yaml_config)
            session.add(new_run)
            session.commit()

        print('Created new run with id={new_run.id}')

        
