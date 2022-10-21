//dev
#include"headers.h"

#include<TDatime.h>
#include<time.h>
#include<TTimer.h>
#include<TText.h>
#include<TSystem.h>

//Mac:" -v Speaker" Speaker = Alex(US)Daniel(GB)Fiona(scotland)Fred(US)Karen(AU)Moira(IE)Rishi(IN)Samantha(US)Tessa(ZA)Veena(IN)Victoria(US) Kyoko(JP)
TString who_say = "Alex";

int x_timer = 172;

// setting file
FILE *fs;

TString dir_picture = "log/picture/";
TString day_of_week[7] = {"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};

//the size of window
// int VerticalSize = 860;
// int HorizontalSize = 1420;
int VerticalSize = 1060;
int HorizontalSize = 1920;

int height_mainframe = 800;
int height_clock = 150*1.2;
int height_slider = 80;
int height_subframe = height_clock+height_slider+4;
int height_task = height_subframe;
int height_setting = 300;
int height_timer;
int height_combo = 20;
int height_checkbutton = 20;
int height_button = 20+8;
int height_timerframe;
int height_chainnum;

int width_timer = 780;
int width_mainframe = x_timer+width_timer;
int width_subframe = (1418 - width_mainframe)*1.2;
int width_setting = 200;
int width_combo = 72;
int width_checkbutton = 68;
int width_button = 45+18+10;
int width_task = HorizontalSize-width_subframe-15;
int width_chainnum;

int inter_button = 10;

int x_fb_start = (x_timer-inter_button-width_button*2)/2;
int x_fb_reset = x_fb_start+width_button+10;
int x_fb_send = 250;
int x_fbr = 310*1.2;
int y_fb_start;
int y_fb_reset;
int y_fb_send = 10;
int y_fbr = 128*1.2;

int x_fcb_skip = 101;
int y_fcb_skip = 23;
int x_skiptime = 101;
int y_skiptime = 2;

int radiobutton_width = 60;
int radiobutton_height = 18;

int count_down;

//the number of Chain
int Chain_num;

//time difference(hour)
int timedif = 0;

//skip(second)
int time_skip = (210*60-0)*0;

//配列に使うやつ
int k;

char url_webhook[128];
char url_dropbox[128];

// limit
const int limit_process = 16;
const int limit_letters = 16;
const int limit_chains = 32;

int length_bar[limit_process];
int sum_length_bar[limit_process];

//作業ごとの時間(min)
int processtime_min[limit_process];

//elapsed time(second)
int processtime[limit_process];

//start time(min)
int processmin[limit_process];

//process
char process[limit_process][limit_letters];
TString process_ev[limit_letters];
TString process_current[limit_letters];

int dt2[limit_chains];
int dt2_compare[limit_chains];
// time_t all_estimated_time[limit_process*limit_chains];
time_t t0[limit_chains];

int state_send_button = 0;
bool state_devtimer[limit_chains];
TGPictureButton *fb_send;
TGPictureButton *fb_url;

class Task : public TGVerticalFrame{
	private:
	int dt2_send[3];
	int Chain_id_send[3];
	int n,l,eh,em,es;
	bool send_if = false;
	bool url_if = false;
	TString process_send[3];
	TString Webhook = Form("%s",url_webhook);
	TDatime ST;
	TText *text;
	TCanvas *c1;
	TTimer *timer;
   	TRootEmbeddedCanvas *fRootEmbeddedCanvas1 = new TRootEmbeddedCanvas(0,this,386,102);
	public:
	Task(TGHorizontalFrame *fMainFrame);
	~Task(){};
	void Compare();
	void Send();
	void URL();
	ClassDef(Task,0)
};

void Task::Compare(){
	int Chain_id[limit_chains];
	for(int i=0;i<Chain_num;i++){
		dt2_compare[i] = dt2[i];
		Chain_id[i] = i+1;
	}
	// comb sort
	int h = Chain_num/1.3;
	for(int i=h;i>0;i--){
		for(int j=0;j<Chain_num-i;j++){
			if(dt2_compare[j]>dt2_compare[j+i]){
				int buffer = dt2_compare[j];
				dt2_compare[j] = dt2_compare[j+i];
				dt2_compare[j+i] = buffer;
				int buffer_id = Chain_id[j];
				Chain_id[j] = Chain_id[j+i];
				Chain_id[j+i] = buffer_id;
			}
		}
	}

	c1->cd();
	c1->Clear();
	c1->Divide(1,1);

	text->SetTextColor(kRed);
	text->SetTextSize(0.2);

	c1->cd(1);
	text->DrawTextNDC(0,0.85,"Task List");
	// text->DrawTextNDC(0,0.85,"___________|");

	text->SetTextSize(0.15);

	double y_firsttask = 0.7;
	double y_othertask = 0.2;
	text->DrawTextNDC(0,y_firsttask-0.1, "1.");
	for(int i=2;i<=3;i++){
		text->DrawTextNDC(0,y_othertask-0.15*(i-2), Form("%d.",i));
	}

	int d = 0;
	for(int i=0;i<Chain_num;i++){
		if(dt2_compare[i]>0){
			text->SetTextSize(0.15);
			c1->cd(1);
			int hc=dt2_compare[i]/3600;
			int mc=(dt2_compare[i]%3600)/60;
			int sc=dt2_compare[i]%60;
			if(d==0){
				text->SetTextSize(0.35);
				text->DrawTextNDC(0,y_firsttask-0.3, Form(" Chain%d  "+process_current[Chain_id[i]-1]+"->"+process_ev[Chain_id[i]-1]+" %02d:%02d:%02d",Chain_id[i],hc,mc,sc));
				// text->DrawTextNDC(0,y_firsttask-0.15,"  "+process_current[Chain_id[i]-1]+" -> "+process_ev[Chain_id[i]-1]);
				// text->SetTextSize(0.275);
				// text->DrawTextNDC(0,y_firsttask-0.415, Form(" %02d:%02d:%02d",hc,mc,sc));
			}
			else{
				text->DrawTextNDC(0,y_othertask-0.15*(d-1), Form("      Chain %d      "+process_current[Chain_id[i]-1]+" -> "+process_ev[Chain_id[i]-1]+"        %02d:%02d:%02d",Chain_id[i],hc,mc,sc));
				// text->DrawTextNDC(0.5*(d-1),y_othertask-0.1, process_current[Chain_id[i]-1]+" -> "+process_ev[Chain_id[i]-1]);
				// text->DrawTextNDC(0.5*(d-1),y_othertask-0.2, Form("      %02d:%02d:%02d",hc,mc,sc));
			}

			dt2_send[d] = dt2_compare[i];
			Chain_id_send[d] = Chain_id[i];
			process_send[d] = process_current[Chain_id[i]-1]+"->"+process_ev[Chain_id[i]-1];

			d++;
		}
		if(d==3){break;}
	}

	text->SetTextSize(0.1);
	if(send_if){
		fb_send->SetState(kButtonDisabled);
		l++;
		text->DrawTextNDC(0.25,0.875,Form("\"Chain%d "+process_send[0]+" %02d:%02d:%02d"+"\"",Chain_id_send[0],eh,em,es));
		if(l==50){
			send_if=false;
			fb_send->SetEnabled();
		}
	}
	else{
		text->DrawTextNDC(0.25,0.875,"First Task");
	}
	if(url_if){
		fb_url->SetState(kButtonDisabled);
		n++;
		// text->DrawTextNDC(0.25,0.85,Form("\"Chain%d "+process_send[0]+" %02d:%02d:%02d"+"\"",Chain_id_send[0],eh,em,es));
		if(n==50){
			url_if=false;
			fb_url->SetEnabled();
		}
	}
	else{
		// text->DrawTextNDC(0.25,0.85,"First Task");
	}
	text->DrawTextNDC(0.65,0.875,"Dropbox Link");

	c1->Modified();
	c1->Update();
}

void Task::Send(){
	time_t t;
	time(&t);
	time_t estimated_time = t+dt2_send[0];
	ST.Set(estimated_time);
	eh = ST.GetHour();
	em = ST.GetMinute();
	es = ST.GetSecond();
	TString time = Form("%02d:%02d:%02d",ST.GetHour(),ST.GetMinute(),ST.GetSecond());
	TString id = Form("Chain%d",Chain_id_send[0]);
	TString tsk = "curl -i -X POST -d 'payload={\"text\": \""+id+" "+process_send[0]+" "+time+"\"}' "+Webhook+" &";
	gSystem->Exec(tsk);
	send_if = true;
	l = 0;
}

void Task::URL(){
	TString Dropbox = Form("%s",url_dropbox);
	TString url = "curl -i -X POST -d 'payload={\"text\": \""+Dropbox+"\"}' "+Webhook+" &";
	gSystem->Exec(url);
	url_if = true;
	n = 0;
}

Task::Task(TGHorizontalFrame *fMainFrame) : TGVerticalFrame(fMainFrame,width_task,height_task,kRaisedFrame){
	

	SetBackgroundColor(TColor::Number2Pixel(kBlack));
   
   	timer = new TTimer(100);
   	timer->Connect("Timeout()","Task", this, "Compare()");
	
	text = new TText();
	text->SetTextSize(0.5);

   	SetLayoutBroken(kTRUE);
   
   	Int_t wfRootEmbeddedCanvas1 = fRootEmbeddedCanvas1->GetCanvasWindowId();
   	c1 = new TCanvas("c", 10, 10, wfRootEmbeddedCanvas1);
   	fRootEmbeddedCanvas1->AdoptCanvas(c1);
   	AddFrame(fRootEmbeddedCanvas1, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,0,0));
   	fRootEmbeddedCanvas1->MoveResize(0,0,width_task,height_task);

	fb_send = new TGPictureButton(this,gClient->GetPicture(dir_picture+"send.png",width_button,height_button));
	fb_send->SetState(kButtonDisabled);
	fb_send->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	TQObject::Connect(fb_send,"Clicked()", "Task", this, "Send()");
   	AddFrame(fb_send, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fb_send->MoveResize(x_fb_send,y_fb_send,width_button,height_button);

	fb_url = new TGPictureButton(this,gClient->GetPicture(dir_picture+"share.png",width_button,height_button));
	fb_url->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	TQObject::Connect(fb_url,"Clicked()", "Task", this, "URL()");
   	AddFrame(fb_url, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fb_url->MoveResize(x_fb_send+500,y_fb_send,width_button,height_button);

	timer->TurnOn();
}

class Setting : public TGVerticalFrame{
	private:
	TText *text;
	TCanvas *c1;
	public:
	Setting(TGHorizontalFrame *fMainFrame);
	~Setting(){};
	ClassDef(Setting,0)
};

Setting::Setting(TGHorizontalFrame *fMainFrame) : TGVerticalFrame(fMainFrame,width_setting,height_setting,kRaisedFrame){

	SetBackgroundColor(TColor::Number2Pixel(kBlack));
	
	text = new TText();
	text->SetTextSize(0.5);

   	SetLayoutBroken(kTRUE);
   
   	TRootEmbeddedCanvas *fRootEmbeddedCanvas1 = new TRootEmbeddedCanvas(0,this,386,102);
   	Int_t wfRootEmbeddedCanvas1 = fRootEmbeddedCanvas1->GetCanvasWindowId();
   	c1 = new TCanvas("c", 10, 10, wfRootEmbeddedCanvas1);
   	fRootEmbeddedCanvas1->AdoptCanvas(c1);
   	AddFrame(fRootEmbeddedCanvas1, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,0,0));
   	fRootEmbeddedCanvas1->MoveResize(0,0,width_setting,height_setting);
	
	c1->cd();
	c1->Clear();
	c1->Divide(2,1);

	c1->cd(1);

	text->SetTextColor(kRed);

	text->SetTextSize(0.2);
	text->DrawTextNDC(0.1,0.9, "Process");
	c1->cd(2);
	text->DrawTextNDC(0.1,0.9, "Time(Min)");
	for(int i=0;i<k;i++){
		c1->cd(1);
		text->DrawTextNDC(0.1,0.75-i*0.1, process[i]);
		c1->cd(2);
		text->DrawTextNDC(0.1,0.75-i*0.1, Form("%d",processtime_min[i]));
	}
	c1->cd();

	TLine *line = new TLine(0,0,0,5);
	TLine *hline = new TLine();
	
	line->SetLineColor(kRed);
	line->SetLineWidth(5);
	line->DrawLineNDC(0.5,0,0.5,1);
	// line->SetLineStyle(3);//1=line,2=broken,3=dotted,4=broken-dot,5=long-broken-dot
	// line->Draw();;
	// c1->cd();
	
	hline->SetLineColor(kRed);
	hline->SetLineWidth(5);
	hline->DrawLineNDC(0,0.85,1,0.85);
	// line->SetLineStyle(3);//1=line,2=broken,3=dotted,4=broken-dot,5=long-broken-dot
	// line->Draw();;


}

class Clock : public TGHorizontalFrame{
	private:
	TTimer *timer;
	time_t t;
	TDatime CT;
	TCanvas *c1,*c2;
	TText *text;
	int twelve = 0;
	TRootEmbeddedCanvas *fRootEmbeddedCanvas1 = new TRootEmbeddedCanvas(0,this,386,102);
	TGRadioButton *fbr1 = new TGRadioButton(this,"AM-PM");
	TGRadioButton *fbr2 = new TGRadioButton(this,"24h");
	public:
	Clock(TGVerticalFrame *fMainFrame);
	~Clock(){if(timer) timer->TurnOff();};
	void OnTimer();
	void SetAmPm(){
		twelve=12;
		fbr2->SetState(kButtonUp);
	}
	void Set24h(){
		twelve=0;
		fbr1->SetState(kButtonUp);
	}
	ClassDef(Clock,0) // Development 
};

void Clock::OnTimer(){
	time(&t);

	c1->cd();
	c1->Clear();

	// Time(hours:minutes)
	text->SetTextSize(0.8);
	text->SetTextColor(kRed);
	CT.Set(t);
	int h = CT.GetHour()-twelve;
	TString meridiem;
	if(CT.GetHour()-twelve>0){
		text->DrawTextNDC(0.1,0.4, Form("%02d:%02d", h,CT.GetMinute()));
		if(twelve==12){meridiem = "PM";}
	}
	else{
		text->DrawTextNDC(0.1,0.4, Form("%02d:%02d", CT.GetHour(),CT.GetMinute()));
		meridiem = "AM";
	}
	text->SetTextSize(0.2);
	text->DrawTextNDC(0.78,0.78,meridiem);

	// Time(seconds)
	text->SetTextSize(0.5);
	text->DrawTextNDC(0.75,0.4, Form("%02d",CT.GetSecond()));

	// Date
	text->SetTextSize(0.3);
	text->DrawTextNDC(0.04,0.15, Form("%02d",CT.GetDay()));
	text->DrawTextNDC(0.23,0.15, Form("%02d",CT.GetMonth()));
	text->DrawTextNDC(0.42,0.15, Form("%04d",CT.GetYear()));

	text->SetTextSize(0.21);
	text->DrawTextNDC(0.65,0.2, day_of_week[CT.GetDayOfWeek()-1]);

	text->SetTextSize(0.15);
	text->DrawTextNDC(0.05,0.04, "Day");
	text->DrawTextNDC(0.22,0.04, "Month");
	text->DrawTextNDC(0.47,0.04, "Year");
	// text->DrawTextNDC(0.6,0, day_of_week[CT.GetDayOfWeek()-1]);

	c1->Modified();
	c1->Update();
	
}

Clock::Clock(TGVerticalFrame *fMainFrame) : TGHorizontalFrame(fMainFrame,width_subframe,height_clock,kRaisedFrame){

	SetBackgroundColor(TColor::Number2Pixel(kBlack));
   
   	timer = new TTimer(100);
   	timer->Connect("Timeout()","Clock", this, "OnTimer()");
	
	text = new TText();
	text->SetTextSize(0.5);

   	SetLayoutBroken(kTRUE);
   
   	Int_t wfRootEmbeddedCanvas1 = fRootEmbeddedCanvas1->GetCanvasWindowId();
   	c1 = new TCanvas(Form("c"), 10, 10, wfRootEmbeddedCanvas1);
   	fRootEmbeddedCanvas1->AdoptCanvas(c1);
   	AddFrame(fRootEmbeddedCanvas1, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,0));
   	fRootEmbeddedCanvas1->MoveResize(0,0,width_subframe,height_clock);

	fbr1->SetBackgroundColor(kBlack);
	fbr1->SetTextColor(TColor::Number2Pixel(kRed+1));
	fbr1->Connect("Clicked()", "Clock", this, "SetAmPm()");
   	AddFrame(fbr1, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fbr1->MoveResize(x_fbr,y_fbr,radiobutton_width+5,radiobutton_height);

	fbr2->SetBackgroundColor(kBlack);
	fbr2->SetTextColor(TColor::Number2Pixel(kRed+1));
	fbr2->Connect("Clicked()", "Clock", this, "Set24h()");
   	AddFrame(fbr1, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fbr2->MoveResize(x_fbr+radiobutton_width+10,y_fbr,radiobutton_width,radiobutton_height);
	fbr2->SetState(kButtonDown);
	
	timer->TurnOn();
}

class Slider : public TGHorizontalFrame{

	private:
	TTimer *timer;
	TText *text;
	TCanvas *c1;
	TGHSlider *hslider;
	public:
	Slider(TGVerticalFrame *fSubFrame);
	~Slider(){};
	void GetValue();
	ClassDef(Slider,0);
};

void Slider::GetValue(){
	count_down = hslider->GetPosition() * 5;

	c1->cd();
	c1->Clear();
	if(count_down==0){text->DrawTextNDC(0.06,0.5,Form("No count down."));}
	else{text->DrawTextNDC(0.06,0.5, Form("Count down from %d seconds before.",count_down));}

	c1->Modified();
	c1->Update();
	// printf("%d\n",count_down);
}

Slider::Slider(TGVerticalFrame *fSubFrame) : TGHorizontalFrame(fSubFrame,width_subframe,height_slider,kRaisedFrame){

	SetBackgroundColor(TColor::Number2Pixel(kBlack));
   
   	timer = new TTimer(100);
   	timer->Connect("Timeout()","Slider", this, "GetValue()");
	
	text = new TText();
	text->SetTextSize(0.34);
	text->SetTextColor(kRed);

   	SetLayoutBroken(kTRUE);

	TRootEmbeddedCanvas *fRootEmbeddedCanvas1 = new TRootEmbeddedCanvas(0,this,386,102);
   	Int_t wfRootEmbeddedCanvas1 = fRootEmbeddedCanvas1->GetCanvasWindowId();
   	c1 = new TCanvas(Form("c"), 10, 10, wfRootEmbeddedCanvas1);
   	fRootEmbeddedCanvas1->AdoptCanvas(c1);
   	AddFrame(fRootEmbeddedCanvas1, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,0));
   	fRootEmbeddedCanvas1->MoveResize(0,0,width_subframe,height_slider);

	hslider = new TGHSlider((TGHorizontalFrame *)this,width_subframe-18,kSlider1|kScaleBoth);
	hslider->SetRange(0,5);
	hslider->SetPosition(2);
	hslider->SetScale(17);
	hslider->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
	AddFrame(hslider,new TGLayoutHints(2,2,2,2));
	hslider->Move(9,50);

	timer->TurnOn();
}

class Event : public TObject{
	
	private :
	int fMin;
	int fSec;
	TString fMessage;
	bool fDone;
	
	public:
	Event(int min, int sec, TString message): fMin(min), fSec(sec), fMessage(message), fDone(kFALSE){}
	~Event(){}
	
	void Check(time_t t){
		if(fDone) return;
		if(t>=fMin*60+fSec){
			TString cmd = "say -v "+who_say+" \""+fMessage+"\""+" &"; 
			printf("%s\n", cmd.Data());
			gSystem->Exec(cmd);
			fDone=kTRUE;
		}
	}
	
	int RemainingTime(time_t t){
		return fMin*60+fSec-t;
	}
	
	int EventTime(){
		return fMin*60+fSec;
	}

	time_t EstimatedTime(){
		return fSec;
	}
	
	void Reset() { fDone=kFALSE;}
	
	const char *GetMessage(){return fMessage.Data();}
};

class DevTimer : public TGHorizontalFrame{
	private:
	TGLabel *fLabelChain;
	TTimer *timer;
	FILE *fp,*fl;
	bool skip_if = false;
	bool reset_if = false;
	int dt;
	int sh[8], sm[8], ss[8], start,skip_log[8];
	time_t t, t1, t2, total;
	TDatime T0,CT;
	int ID;
	int dt3;
	int s=0;
	int d=0;
	float p=0;
	TCanvas *c1,*c2;
	TText *text;
	TObjArray *fMsgEvents;
	TObjArray *fTimeEvents;
	TObjArray *fHPBar;
	TRootEmbeddedCanvas *fRootEmbeddedCanvas1 = new TRootEmbeddedCanvas(0,this,386,102);
	TGHProgressBar *fpb;
	TGComboBox *fCombo = new TGComboBox(this,kSunkenFrame);
	TGPictureButton *fb_start = new TGPictureButton(this,gClient->GetPicture(dir_picture+"start.png",width_button,height_button));
	// TGPictureButton *fb_reset = new TGPictureButton(this,"Reset");
	TGPictureButton *fb_reset = new TGPictureButton(this,gClient->GetPicture(dir_picture+"reset.png",width_button,height_button));
	TGCheckButton *fcb_skip = new TGCheckButton(this,"Recover");
	public:
	DevTimer(TGVerticalFrame *fMainFrame, int id);
	~DevTimer(){if(timer) timer->TurnOff();};
	void SpeechTime(time_t t);
	void OnTimer();
	void Start();
	void Skip();
	void Reset();
	void Unlock();
	void Lock();
	void Set0();
	ClassDef(DevTimer,0) // Development 
};

DevTimer::DevTimer(TGVerticalFrame *fMainFrame, int id) : TGHorizontalFrame(fMainFrame,width_mainframe,height_timerframe,kRaisedFrame), ID(id){

	SetBackgroundColor(TColor::Number2Pixel(kBlack));
	fTimeEvents = new TObjArray;
	for(int i=0;i <= k;i++){
		fTimeEvents->Add( new Event(processmin[i],0,process[i]));
	}
	
	fMsgEvents = new TObjArray;

	TString chain   = Form("Chain %d. ",id);
	TString in1min  = "in one minute.";
	TString in3min  = "in three minutes.";
	TString in30sec = "in thirty seconds.";
	TString in10sec = "in ten seconds.";

	TString in_i_sec[32];
	for(int i=1;i<30;i++){in_i_sec[i]=Form("%d.",i);}

	TString start   = "start";

	for(int i=1;i<=k;i++){
		for(int j=29;j>0;j--){
			fMsgEvents->Add( new Event(processmin[i], -j, in_i_sec[j]));
		}
		fMsgEvents->Add( new Event(processmin[i], -60, chain+" "+process[i]+" "+in1min));
		fMsgEvents->Add( new Event(processmin[i], -30, chain+" "+process[i]+" "+in30sec));
		if(i!=k){
			fMsgEvents->Add( new Event(processmin[i],   0, start+" "+process[i]+" "+chain));
		}
		else{
			fMsgEvents->Add( new Event(processmin[i],   0, chain+process[i]));
		}

		// printf("%d\n",fMsgEvents->GetEntries());
	}

	fMsgEvents->Add( new Event(processmin[0],   0, start+" "+process[0]+" "+chain));
   
   	timer = new TTimer(100);
   	timer->Connect("Timeout()","DevTimer", this, "OnTimer()");
	
	text = new TText();
	text->SetTextSize(0.5);

   	SetLayoutBroken(kTRUE);
	
	TString num = Form("%d",ID);
	TGIcon *fnum = new TGIcon(this,gClient->GetPicture(dir_picture+"chain/"+ID+".png",height_chainnum,height_chainnum),height_chainnum,height_chainnum);
	AddFrame(fnum, new TGLayoutHints(0,0,0,0));

   	fLabelChain = new TGLabel(this,"Start at");
   	fLabelChain->SetTextJustify(36);
   	fLabelChain->SetMargins(0,0,0,0);
   	fLabelChain->SetWrapLength(-1);
   	fLabelChain->SetBackgroundColor(TColor::Number2Pixel(kBlack));
   	fLabelChain->SetTextColor(TColor::Number2Pixel(kRed+1));
   	AddFrame(fLabelChain, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fLabelChain->MoveResize(width_chainnum-3,5,44,20);

	fCombo->ChangeBackground(TColor::Number2Pixel(kRed+1));
	fCombo->SetEnabled();
	fCombo->MoveResize(x_skiptime,y_skiptime,width_combo,height_combo);
	TString a = Form("%d",ID);
	fp = fopen("log/skiplog/Chain"+a+".log","r");
	while(fscanf(fp,"%d %02d %02d %02d",&skip_log[s],&sh[s],&sm[s],&ss[s]) !=EOF){
    	fCombo->AddEntry(Form("%02d:%02d:%02d",sh[s],sm[s],ss[s]), s);
		s++;
	}
	fclose(fp);
	fCombo->Select(0);
	AddFrame(fCombo, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));

   	//Button
   	fb_start->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	TQObject::Connect(fb_start,"Clicked()", "DevTimer", this, "Start()");
   	AddFrame(fb_start, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fb_start->MoveResize(x_fb_start,y_fb_start,width_button,height_button);

	fb_reset->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	TQObject::Connect(fb_reset,"Clicked()", "DevTimer", this, "Reset()");
   	AddFrame(fb_reset, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fb_reset->MoveResize(x_fb_reset,y_fb_reset,width_button,height_button);
	fb_reset->SetState(kButtonDisabled);

	fcb_skip->SetBackgroundColor(TColor::Number2Pixel(kBlack));
	fcb_skip->SetTextColor(TColor::Number2Pixel(kRed+1));
	TQObject::Connect(fcb_skip,"Clicked()", "DevTimer", this, "Skip()");
	AddFrame(fcb_skip, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fcb_skip->MoveResize(x_fcb_skip,y_fcb_skip,width_checkbutton,height_checkbutton);
	if(s==0){fcb_skip->SetState(kButtonDisabled);}
   
   	Int_t wfRootEmbeddedCanvas1 = fRootEmbeddedCanvas1->GetCanvasWindowId();
   	c1 = new TCanvas(Form("c%d_1",ID), 10, 10, wfRootEmbeddedCanvas1);
   	fRootEmbeddedCanvas1->AdoptCanvas(c1);
   	AddFrame(fRootEmbeddedCanvas1, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fRootEmbeddedCanvas1->MoveResize(x_timer,0,width_timer,height_timer);

	fHPBar = new TObjArray;
	for(int i=0;i<k;i++){
		length_bar[i] = (width_timer-20)*processtime_min[i]/processmin[k];
		sum_length_bar[i+1] = length_bar[i] + sum_length_bar[i];
		fHPBar->Add( new TGHProgressBar(this,TGProgressBar::kStandard,length_bar[i]));
	}
	for(int i=0;i<k;i++){
		fpb = (TGHProgressBar *)fHPBar->At(i);
		fpb->SetBarColor(TColor::Number2Pixel(kRed+1));
		fpb->Move(sum_length_bar[i]+x_timer+2*i,height_timer);
		fpb->ShowPosition();
		fpb->ChangeBackground(TColor::Number2Pixel(kBlack));
		fpb->SetForegroundColor(TColor::Number2Pixel(kOrange));
	}

	state_devtimer[ID-1] = false;
}

void DevTimer::SpeechTime(time_t t){
	char buf[256];
	sprintf(buf, "say -v "+who_say+" %02d:%02d:%02d", T0.GetHour(), T0.GetMinute(), T0.GetSecond());
	gSystem->Exec(buf);
}

void DevTimer::OnTimer(){
	time(&t);
	if(reset_if){t = t0[ID-1];}
	dt= t-t0[ID-1]+time_skip;
	int h=dt/3600;
	int m=(dt%3600)/60;
	int sec=dt%60;
	c1->cd();
	c1->Clear();
	c1->Divide(2,1);
	c1->cd(1);
	text->SetTextSize(1.0);
	text->SetTextColor(kRed);
	d++;
	if(reset_if){text->DrawTextNDC(0.02,0.1, " "); dt2[ID-1]=0;}
	else{
		text->DrawTextNDC(0.02,0.1, Form("%02d:%02d:%02d", h,m,sec));
		text->SetTextSize(0.5);
		c1->cd(2);
		Event *ev_current = NULL;
		Event *ev_next = NULL;
		for(int i=0;i<fTimeEvents->GetEntries();i++){
			Event *ev = (Event *)fTimeEvents->At(i);
			dt2[ID-1] = ev->RemainingTime(dt);
			if(dt2[ID-1]>0){
				int h2=dt2[ID-1]/3600;
				int m2=(dt2[ID-1]%3600)/60;
				int s2=dt2[ID-1]%60;
				process_ev[ID-1]=ev->GetMessage();
				text->DrawTextNDC(0.02,0.1,Form("%02d:%02d:%02d to %s", h2,m2,s2,ev->GetMessage()));
				ev_next = ev;

				dt3 = -ev_current->RemainingTime(dt);
				int h3=dt3/3600;
				int m3=(dt3%3600)/60;
				int s3=dt3%60;
				process_current[ID-1]=ev_current->GetMessage();
				text->DrawTextNDC(0.02,0.55,Form("%02d:%02d:%02d %s", h3,m3,s3, ev_current->GetMessage()));

				p = dt3*100/(processtime_min[i-1]*60);
				fpb = (TGHProgressBar *)fHPBar->At(i-1);
				fpb->SetPosition(p);
				if(dt2[ID-1]==1){fpb->SetPosition(100);}

				break;
			}
			ev_current = ev;
			if(i>0){
				fpb = (TGHProgressBar *)fHPBar->At(i-1);
				fpb->SetPosition(100);
			}
		}
		if(NULL==ev_next){
			text->DrawTextNDC(0.02,0.1,"     since finished");

			int dt3 = -ev_current->RemainingTime(dt);
			int h3=dt3/3600;
			int m3=(dt3%3600)/60;
			int s3=dt3%60;
			text->DrawTextNDC(0.02,0.55,Form("%02d:%02d:%02d passed", h3,m3,s3));
		}
	}

	c1->Modified();
	c1->Update();
	
	for(int i=0;i<fMsgEvents->GetEntries();i++){
		if(i%32>=29-count_down){
			Event *ev = (Event *)fMsgEvents->At(i);
			ev->Check(dt);
		}
	}
}

void DevTimer::Start(){
	TQObject::Disconnect(fb_start);

	TString a = Form("%d.",ID);
	gSystem->Exec("say -v "+who_say+" \"start "+process[0]+" Chain"+a+"\""+" &");

	state_send_button++;
	if(state_send_button!=0){fb_send->SetEnabled();}
	
   	time(&t0[ID-1]);
	if(skip_if){t0[ID-1] = start;}

   	T0.Set(t0[ID-1]);

	for(int i=0;i<fTimeEvents->GetEntries();i++){
		Event *ev = (Event *)fTimeEvents->At(i);
		ev->Reset();
	}
	for(int i=0;i<fMsgEvents->GetEntries();i++){
		Event *ev = (Event *)fMsgEvents->At(i);
		ev->Reset();
	}
	
	timer->TurnOn();

	if(!skip_if){
		TString id = Form("%d",ID);
		remove("log/skiplog/Chain"+id+".log");
		fp = fopen("log/skiplog/Chain"+id+".log", "at");
		fprintf(fp, "%ld %02d %02d %02d\n", t0[ID-1], T0.GetHour(), T0.GetMinute(), T0.GetSecond());
		if(s<5){
			for(int i=0;i<s;i++){
				fprintf(fp,"%d %02d %02d %02d\n",skip_log[i],sh[i],sm[i],ss[i]);
			}
		}
		else{
			for(int i=0;i<4;i++){
				fprintf(fp,"%d %02d %02d %02d\n",skip_log[i],sh[i],sm[i],ss[i]);
			}
		}
		fclose(fp);

		fCombo->RemoveAll();
		s = 0;
		fp = fopen("log/skiplog/Chain"+id+".log","r");
		while(fscanf(fp,"%d %02d %02d %02d",&skip_log[s],&sh[s],&sm[s],&ss[s]) !=EOF){
    		fCombo->AddEntry(Form("%02d:%02d:%02d",sh[s],sm[s],ss[s]), s);
			s++;
		}
		fclose(fp);
		fCombo->Select(0);
	}
	fCombo->SetEnabled(kFALSE);

	// if(!skip_if){
	// 	TString id = Form("%d",ID);
	// 	TString Date  =	Form("%d",T0.GetDate());
	// 	TString Hour  =	Form("%02d",T0.GetHour());
	// 	TString Minute=	Form("%02d",T0.GetMinute());
	// 	TString Second=	Form("%02d",T0.GetSecond());
	// 	remove("log/skiplog/Chain"+id+".log");
   	// 	fl = fopen("log/Chain"+id+"/"+Date+":"+Hour+":"+Minute+":"+Second+".log", "at");
	// 	fprintf(fl, "Start Developing at %02d:%02d:%02d\n", T0.GetHour(), T0.GetMinute(), T0.GetSecond());
	// 	fclose(fl);
	// }
	
   	fprintf(stdout, "Chain %2d, Start at %02d:%02d:%02d\n", ID, T0.GetHour(), T0.GetMinute(), T0.GetSecond());
   	
	reset_if = false;

	fcb_skip->SetState(kButtonDisabled);
	state_devtimer[ID-1] = true;

	fb_start->SetPicture(gClient->GetPicture(dir_picture+"unlock.png",width_button,height_button));
	TQObject::Connect(fb_start,"Clicked()", "DevTimer", this, "Unlock()");
}

void DevTimer::Skip(){
	TQObject::Disconnect(fcb_skip);

	start = skip_log[fCombo->GetSelected()];
	skip_if = true;
	printf("%d",start);
	printf("a");

	TQObject::Connect(fcb_skip,"Clicked()", "DevTimer", this, "Set0()");
}

void DevTimer::Reset(){
	TQObject::Disconnect(fb_start);
	TQObject::Disconnect(fcb_skip);
	state_send_button--;
	if(state_send_button==0){fb_send->SetState(kButtonDisabled);}

	for(int i=0;i<k;i++){
		fpb = (TGHProgressBar *)fHPBar->At(i);
		// fpb->SetPosition(0);
		fpb->Reset();
	}

	fcb_skip->SetEnabled();
	fCombo->SetEnabled();

	fb_reset->SetState(kButtonDisabled);
	reset_if = true;

	fcb_skip->SetState(kButtonUp);
	skip_if = false;

	state_devtimer[ID-1] = false;

	fb_start->SetPicture(gClient->GetPicture(dir_picture+"start.png",width_button,height_button));
	TQObject::Connect(fb_start,"Clicked()", "DevTimer", this, "Start()");
	TQObject::Connect(fcb_skip,"Clicked()", "DevTimer", this, "Skip()");
}

void DevTimer::Unlock(){
	TQObject::Disconnect(fb_start);

	fb_reset->SetEnabled();

	fb_start->SetPicture(gClient->GetPicture(dir_picture+"lock.png",width_button,height_button));
	TQObject::Connect(fb_start,"Clicked()", "DevTimer", this, "Lock()");
}

void DevTimer::Lock(){
	TQObject::Disconnect(fb_start);

	fb_reset->SetState(kButtonDisabled);

	fb_start->SetPicture(gClient->GetPicture(dir_picture+"unlock.png",width_button,height_button));
	TQObject::Connect(fb_start,"Clicked()", "DevTimer", this, "Unlock()");
}

void DevTimer::Set0(){
	TQObject::Disconnect(fcb_skip);

	skip_if = false;

	TQObject::Connect(fcb_skip,"Clicked()", "DevTimer", this, "Skip()");
}

class DevTimers : public TGMainFrame {
	private:
	TObjArray eTimers;
	
	public:
	DevTimers();
	~DevTimers(){for(int i=0; i<eTimers.GetEntries(); i++){delete eTimers.At(i);}}
	
	ClassDef(DevTimers,0) // Development 
};

DevTimers::DevTimers() : TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame){
	
	SetBackgroundColor(TColor::Number2Pixel(kRed+1));

	TGHorizontalFrame *frame1= new TGHorizontalFrame((TGMainFrame *)this,0,0,kSunkenFrame|kHorizontalFrame);
	AddFrame(frame1,new TGLayoutHints(kLHintsExpandX | kLHintsTop,0,0,0,0));
	frame1->SetBackgroundColor(TColor::Number2Pixel(kRed+1));

	TGVerticalFrame *frame1_1= new TGVerticalFrame((TGHorizontalFrame *)frame1,width_mainframe,height_mainframe,kSunkenFrame);
	frame1->AddFrame(frame1_1,new TGLayoutHints(kLHintsLeft,0,0,0,0));
	// frame1_1->SetLayoutBroken(kTRUE);
	frame1_1->SetBackgroundColor(TColor::Number2Pixel(kRed+1));

	TGVerticalFrame *frame1_2= new TGVerticalFrame((TGHorizontalFrame *)frame1,width_mainframe,height_mainframe,kSunkenFrame);
	frame1->AddFrame(frame1_2,new TGLayoutHints(kLHintsLeft,0,0,0,0));
	// frame1_2->SetLayoutBroken(kTRUE);
	frame1_2->SetBackgroundColor(TColor::Number2Pixel(kRed+1));

	for(int i=1; i<=Chain_num; i++){
		if(i<=Chain_num/2+Chain_num%2){DevTimer *t = new DevTimer((TGVerticalFrame *)frame1_1, i);frame1_1->AddFrame( t,  new TGLayoutHints(kLHintsTop,2,0,2,0));}
		else{DevTimer *t = new DevTimer((TGVerticalFrame *)frame1_2, i);frame1_2->AddFrame( t,  new TGLayoutHints(kLHintsTop,2,0,2,0));}
		// eTimers.Add(t);
		// printf("%d",i);
	}






	TGHorizontalFrame *frame2= new TGHorizontalFrame((TGMainFrame *)this,0,0,kSunkenFrame|kHorizontalFrame);
	AddFrame(frame2,new TGLayoutHints(kLHintsExpandX|kLHintsTop,0,0,0,0));
	frame2->SetBackgroundColor(TColor::Number2Pixel(kRed+1));


	TGVerticalFrame *frame2_1= new TGVerticalFrame((TGHorizontalFrame *)frame2,0,0,kSunkenFrame|kVerticalFrame);
	frame2->AddFrame(frame2_1,new TGLayoutHints(kLHintsExpandY|kLHintsLeft,0,0,0,0));
	frame2_1->SetBackgroundColor(TColor::Number2Pixel(kRed+1));

	Clock *c = new Clock((TGVerticalFrame *)frame2_1);
	frame2_1->AddFrame(c,new TGLayoutHints(kLHintsTop,2,2,2,2));

	Slider *CountSlider = new Slider((TGVerticalFrame *)frame2_1);
	frame2_1->AddFrame(CountSlider,new TGLayoutHints(kLHintsTop,2,2,2,2));

	Task *task = new Task((TGHorizontalFrame *)frame2);
	frame2->AddFrame(task,new TGLayoutHints(kLHintsLeft,2,2,2,2));
	


	// TGHorizontalFrame *frame2_1 = new TGHorizontalFrame((TGVerticalFrame *)frame2,0,0,kSunkenFrame|kVerticalFrame);
	// frame2->AddFrame(frame2_1,new TGLayoutHints(kLHintsExpandX|kLHintsTop,0,0,0,0));
	// frame2_1->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
	

	// Setting *s = new Setting((TGHorizontalFrame *)frame2_1);
	// frame2_1->AddFrame(s,new TGLayoutHints(kLHintsExpandY|kLHintsLeft,2,2,2,2));


	SetMWMHints(kMWMDecorAll,kMWMFuncAll,kMWMInputModeless);
	MapSubwindows();

	Resize(GetDefaultSize());
	MapWindow();
	Resize(HorizontalSize,VerticalSize);
}










void developtimer()
{
	// for(int i=10;i>0;i--){
	// 	printf("%d",i);
	// }
	k = 0;
	fs = fopen("setting.txt","r");
	fscanf(fs,"The number of chains:%d",&Chain_num);
	fscanf(fs,"%s",url_webhook);
	printf("Webhook:%s\n",url_webhook);
	fscanf(fs,"%s",url_dropbox);
	printf("Dropbox:%s\n",url_dropbox);
	printf("The number of chains:%d\n",Chain_num);
	printf("-----------------------\n");
	printf("| Time  | Process\n");
	printf("-----------------------\n");
	while(fscanf(fs,"%d %s",&processtime_min[k],process[k]) != EOF){
		if(k==0){
			processtime[k] = processtime_min[k]*60;
			processmin[k] = 0;
		}
		else{
			processtime[k] = processtime_min[k]*60+processtime[k-1];
			processmin[k] = processmin[k-1]+processtime_min[k-1];
		}
		printf("| %dmin | %s\n",processtime_min[k],process[k]);
		k++;
	}
	printf("-----------------------\n");
	processmin[k] = processmin[k-1]+processtime_min[k-1];
	process[k][0] = 'f'; process[k][1] = 'i'; process[k][2] = 'n'; process[k][3] = 'i'; process[k][4] = 's'; process[k][5] = 'h';
	fclose(fs);

	// printf("processtime_min  processtime  processmin  process\n");
	// for(int i=0;i<=k;i++){
	// 	printf("%d  %d  %d  %s\n",processtime_min[i],processtime[i],processmin[i],process[i]);
	// }
	gStyle->SetCanvasColor(kBlack);
	height_timerframe = height_mainframe/(Chain_num/2+Chain_num%2)-2;
	height_timer = height_timerframe-20;
	if(height_timer>100){height_timer=100;}
	// printf("%d",height_timer);
	y_fb_start = height_timer+20-height_button-4;
	y_fb_reset = y_fb_start;

	FILE *fl;
    for(int i=1;i<=Chain_num;i++){
        TString k = Form("%d",i);
        fl = fopen("log/skiplog/Chain"+k+".log","a");
        fclose(fl);
    }
	height_chainnum = height_timer + 20 - height_button - 10;
	width_chainnum = height_chainnum;
	new DevTimers;
}  