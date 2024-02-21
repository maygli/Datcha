import {HttpProcessor} from "./http_processor.js";

class Updater{
    static _m_Updater = null;
    static getUpdater(){
        if( Updater._m_Updater == null ){
            Updater._m_Updater = new Updater();
        }
        return this._m_Updater;
    }

    constructor(){
        this._m_UpdaterMap=new Map();
        this._m_Interval = 1000;
        this._m_TimerId = setInterval(this._onInterval.bind(this), this._m_Interval);
    }

    setUpdateInterval(theInterval){
        clearTimeout(this._m_TimerId);
        this._m_Interval = theInterval;
        this._m_TimerId = setInterval(this._onInterval.bind(this), this._m_Interval);
    }

    registerClient(theUrl, theUpdateFunc){
        if( this._m_UpdaterMap.has(theUrl) ){
            this._m_UpdaterMap.get(theUrl).push(theUpdateFunc);
        }
        else{
            this._m_UpdaterMap.set(theUrl, [theUpdateFunc]);
        }
        this._m_UpdaterMap.append
    }

    async _readData(theCBs, theURL){
        let aData = await HttpProcessor.loadData(theURL, false);
        theCBs.forEach((aCB)=>{aCB(theURL, aData)});
    }

    async _onInterval(){
        this._m_UpdaterMap.forEach((value,key)=>{ this._readData(value, key);});
    }
};

export {Updater}