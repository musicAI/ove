#include "ove.h"
//#include <iostream>



MeasureToTick* mtt_ = new MeasureToTick();
OVE::OveSong* ove_ = 0;

void print(QString &s){
	printf(s.toStdString().c_str());
	printf("\n");
}

void convertHeader() {
      //VBox* vbox = 0;
      QList<QString> titles = ove_->getTitles();
      if( !titles.empty() && !titles[0].isEmpty() ) {
            QString title = titles[0];
            //score_->setMetaTag("movementTitle", title);
            //addText(vbox, score_, title, Tid::TITLE);
            print(title);
      }

      QList<QString> copyrights = ove_->getCopyrights();
      if( !copyrights.empty() && !copyrights[0].isEmpty() ) {
            QString copyright = copyrights[0];
            //score_->setMetaTag("copyright", copyright);
            print(copyright);
      }

      QList<QString> annotates = ove_->getAnnotates();
      if( !annotates.empty() && !annotates[0].isEmpty() ) {
            QString annotate = annotates[0];
            //addText(vbox, score_, annotate, Tid::POET);
            print(annotate);
      }

      QList<QString> writers = ove_->getWriters();
      if(!writers.empty()) {
            QString composer = writers[0];
            //score_->setMetaTag("composer", composer);
            //addText(vbox, score_, composer, Tid::COMPOSER);
            print(composer);
      }

      if(writers.size() > 1) {
            QString lyricist = writers[1];
            //addText(vbox, score_, lyricist, Tid::POET);
            print(lyricist);
      }

      // if (vbox) {
      //       vbox->setTick(0);
      //       //score_->measures()->add(vbox);
      //       }
}

void createStructure() {
      for(int i=0; i<ove_->getPartCount(); ++i ){
            int partStaffCount = ove_->getStaffCount(i) ;
            //Part* part = new Part(score_);

            for(int j=0; j<partStaffCount; ++j){
                  OVE::Track* track = ove_->getTrack(i, j);
                  //Staff* staff = new Staff(score_);
                  //staff->setPart(part);

                  //part->staves()->push_back(staff);
                  //score_->staves().push_back(staff);
            }

            //score_->appendPart(part);
            //part->setStaves(partStaffCount);
      }

      for(int i = 0; i <ove_->getMeasureCount(); ++i) {
            //Measure* measure  = new Measure(score_);
            int tick = mtt_->getTick(i, 0);
            //measure->setTick(tick);
            //measure->setNo(i);
            //score_->measures()->add(measure);
      }
}

void convertGroups() {
      int i;
      int staffCount = 0;
      //const QList<Part*>& parts = score_->parts();
      for(i=0; i<ove_->getPartCount(); ++i ){
            int partStaffCount = ove_->getStaffCount(i);
            //if(parts == 0)
            //	continue;
            // Part* part = parts.at(i);
            // if(part == 0)
            //       continue;

            for(int j=0; j<partStaffCount; ++j){
                  int staffIndex = staffCount + j;
                  // Staff* staff = score_->staff(staffIndex);
                  // if(staff == 0)
                  //       continue;

                  // brace
                  // if( j == 0 && partStaffCount == 2 ) {
                  //       staff->setBracketType(0, BracketType::BRACE);
                  //       staff->setBracketSpan(0, 2);
                  //       staff->setBarLineSpan(2);
                  //       }

                  // bracket
                  OVE::Staff* staffPtr = getStaff(ove_, staffIndex);
                  if(staffPtr != 0 && staffPtr->getGroupType() == OVE::GroupType::Bracket) {
                        int span = staffPtr->getGroupStaffCount() + 1;
                        int endStaff = staffIndex + span;
                        if(span > 0 && endStaff >= staffIndex && endStaff <= ove_->getTrackCount()) {
                              //staff->addBracket(new BracketItem(staff->score(), BracketType::NORMAL, span));
                              //staff->setBarLineSpan(span);
                           }
                   }
              }

            staffCount += partStaffCount;
            }
      }


void convertLineBreak(){
      for (int j = 0; j <ove_->getMeasureCount(); ++j) {
      		// if (mb->type() != ElementType::MEASURE)
        //           continue;
        //     Measure* measure = static_cast<Measure*> (mb);
      

            for (int i = 0; i < ove_->getLineCount(); ++i) {
                  OVE::Line* line = ove_->getLine(i);
                  if (j > 0) { // measure no.
                        if ((int)line->getBeginBar() + (int)line->getBarCount()-1 == j) {
                              //LayoutBreak* lb = new LayoutBreak(score_);
                              //lb->setTrack(0);
                              //lb->setLayoutBreakType(LayoutBreak::Type::LINE);
                              //measure->add(lb);
                              }
                        }
                  }
            }
      }


int main(int argc, char *argv[]){
	QString name(argv[1]);
	OVE::IOVEStreamLoader* oveLoader = OVE::createOveStreamLoader();
    OVE::OveSong oveSong;
    //QFileInfo fi(name);
    //QString csl  = fi.suffix().toLower();
    QFile oveFile(name);
    if(!oveFile.exists())
        return -1;
    if (!oveFile.open(QFile::ReadOnly)) {
            return -2;
    }
    QByteArray buffer = oveFile.readAll();
    oveFile.close();

	oveSong.setTextCodecName(QString("GBK")); // default charset
	oveLoader->setOve(&oveSong);
	oveLoader->setFileStream((unsigned char*) buffer.data(), buffer.size());
	bool result = oveLoader->load();
	oveLoader->release();

	if(result){
		printf("no error\n");
        //Ms::OveToMScore otm;
        //otm.convert(&oveSong, score);

            //		score->connectSlurs();
    }else{

    	printf("error\n");
    	return -3;
    }

    ove_ = &oveSong;
    
    mtt_->build(ove_, ove_->getQuarter());

    convertHeader();
    // createStructure();
    convertGroups();
    // convertSignatures();
    convertLineBreak();

    int staffCount = 0;
    int partCount = ove_->getPartCount();
    int measureCount = ove_->getMeasureCount();
    for(int i=0; i<partCount; ++i ){
            int partStaffCount = ove_->getStaffCount(i) ;
            printf("part staff: %d\n", partStaffCount);
            //Part* part = score_->parts().at(i);

            for(int j=0; j<partStaffCount; ++j){
                  OVE::Track* track = ove_->getTrack(i, j);

                  //convertTrackHeader(track, part);
                  }

            staffCount += partStaffCount;
         }

      //convertMeasures();

      // convert elements by ove track sequence
      staffCount = 0;
      for(int i=0; i<ove_->getPartCount(); ++i ){
            int partStaffCount = ove_->getStaffCount(i) ;

            for(int j=0; j<partStaffCount; ++j){
                  int trackIndex = staffCount + j;

                  //convertTrackElements(trackIndex);
            }

        staffCount += partStaffCount;
      }

      printf("staffCount %d; partCount %d; measureCount %d\n", staffCount, partCount, measureCount);

      //clearUp();


	return 0;
}