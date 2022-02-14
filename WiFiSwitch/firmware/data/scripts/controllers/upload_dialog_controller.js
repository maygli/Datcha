import {BaseController} from "./base_controller.js"

class UploadDialogController extends BaseController{
    constructor(theEl){
        super(theEl);
        this._m_CloseButton = this._m_BaseElement.querySelector('[data-controller_item="CloseButton"]');
        this._m_CloseButton.style.display = "none";
        this._m_CloseButton.addEventListener("click", this._onClose.bind(this));
        this._m_ProgressBar = this._m_BaseElement.querySelector('[data-controller_item="ProgressBar"]'); 
        this._m_FileNameLbl = this._m_BaseElement.querySelector('[data-controller_item="FileName"]'); 
        this._m_StatusLbl = this._m_BaseElement.querySelector('[data-controller_item="StatusLbl"]');
        this._m_LoadCurrent = this._m_BaseElement.querySelector('[data-controller_item="LoadCurrent"]');      
        this._m_LoadTotal = this._m_BaseElement.querySelector('[data-controller_item="LoadTotal"]');      
        this._m_UploadMsg = this._m_BaseElement.querySelector('[data-controller_item="UploadMsg"]');      
    }

    hide(){
        super.hide();
        if( this._m_CloseDialogCallback ){
            this._m_CloseDialogCallback();
        }
    }

    setCloseDialogCallback(theCallback){
        this._m_CloseDialogCallback = theCallback;
    }

    startUpload(theFileName){
        super.show();
        this._m_FileNameLbl.innerHTML = theFileName;
        this._m_CloseButton.style.display = "none";        
        this._m_StatusLbl.className = "normal_msg";
        this._m_UploadMsg.hidden = false; 
    }

    setProgress(theProgress, theTotal){
        this._m_ProgressBar.style.display="block";
        let aPercent = (theProgress / theTotal) * 100;
        this._m_ProgressBar.value = Math.round(aPercent);
        this._m_StatusLbl.innerHTML = Math.round(aPercent) + i18next.t("settings_upgrade_percents");
        this._m_StatusLbl.className = "normal_msg"; 
        this._m_LoadTotal.innerHTML = theTotal;
        this._m_LoadCurrent.innerHTML = theProgress;
    }

    setError(theErrorMsg){
        this._m_ProgressBar.style.display="none";
        this._m_StatusLbl.innerHTML = theErrorMsg;
        this._m_StatusLbl.className = "error_msg";
        this._m_CloseButton.style.display = "block";        
        this._m_UploadMsg.hidden = true; 
    }

    setSuccess(theSuccessMsg){
        this._m_CloseButton.style.display = "block"; 
        this._m_ProgressBar.style.display="none";
        this._m_StatusLbl.innerHTML = theSuccessMsg;
        this._m_StatusLbl.className = "success_msg";
        this._m_CloseButton.style.display = "block";        
    }

    _onClose(){
        this.hide();
    }
}

export {UploadDialogController};