class BinOM {
    constructor(data) {
        
    }
};

(()=>{
    function isInt(n){
        return Number(n) === n && n % 1 === 0;
    }
    
    function isFloat(n){
        return Number(n) === n && n % 1 !== 0;
    }

    BinOM.Byte = class Byte {
        constructor(value) {
            if(typeof value != "number")
                throw Error("Invalid value type");
            let data = new Uint8Array(1);
            data[0] = value;
            Object.defineProperties(this, {
                data: {
                    get: () => data[0],
                    set: value => data[0] = value
                }
            });
        }

        valueOf() {return this.data;}
    };

    BinOM.Word = class Word {
        constructor(value) {
            if(typeof value != "number")
                throw Error("Invalid value type");
            let data = new Uint16Array(1);
            data[0] = value;
            Object.defineProperties(this, {
                data: {
                    get: () => data[0],
                    set: value => data[0] = value
                }
            });
        }

        valueOf() {return this.data;}
    };

    BinOM.DWord = class DWord {
        constructor(value) {
            if(typeof value != "number")
                throw Error("Invalid value type");
            let data = new Uint32Array(1);
            data[0] = value;
            Object.defineProperties(this, {
                data: {
                    get: () => data[0],
                    set: value => data[0] = value
                }
            });
        }

        valueOf() {return this.data;}
    };

    BinOM.QWord = class QWord {
        constructor(value) {
            if(typeof value == "number")
                value = BigInt(value)
            else if(typeof value != "bigint")
                throw Error("Invalid value type");
            let data = new BigUint64Array(1);
            data[0] = value;
            Object.defineProperties(this, {
                data: {
                    get: () => data[0],
                    set: value => data[0] = BigInt(value)
                }
            });
        }

        valueOf() {return this.data;}
    };

    BinOM.ByteArray = class ByteArray {};
    BinOM.WordArray = class WordArray {};
    BinOM.DWordArray = class DWordArray {};
    BinOM.QWordArray = class QWordArray {};
    BinOM.Array = class BinOMArray {};
    BinOM.Object = class BinOMObject {};

    Object.defineProperties(BinOM.Byte, {type_number:{get: () => 0x01}});
    Object.defineProperties(BinOM.Word, {type_number:{get: () => 0x02}});
    Object.defineProperties(BinOM.DWord, {type_number:{get: () => 0x03}});
    Object.defineProperties(BinOM.QWord, {type_number:{get: () => 0x04}});
    Object.defineProperties(BinOM.ByteArray, {type_number:{get: () => 0x05}});
    Object.defineProperties(BinOM.WordArray, {type_number:{get: () => 0x06}});
    Object.defineProperties(BinOM.DWordArray, {type_number:{get: () => 0x07}});
    Object.defineProperties(BinOM.QWordArray, {type_number:{get: () => 0x08}});
    Object.defineProperties(BinOM.Array, {type_number:{get: () => 0x09}});
    Object.defineProperties(BinOM.Object, {type_number:{get: () => 0x0A}});

})()

const ui8 = BinOM.Byte;
const ui16 = BinOM.Word;
const ui32 = BinOM.DWord;
const ui64 = BinOM.QWord;