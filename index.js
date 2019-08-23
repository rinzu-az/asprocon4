"use strict";
var framework;
(function (framework) {
    var FileParser = /** @class */ (function () {
        function FileParser(filename, content) {
            this.filename = filename;
            this.content = [];
            for (var _i = 0, _a = content.split('\n'); _i < _a.length; _i++) {
                var line = _a[_i];
                var words = line.trim().split(new RegExp('\\s+'));
                this.content.push(words);
            }
            this.y = 0;
            this.x = 0;
        }
        FileParser.prototype.isEOF = function () {
            return this.content.length <= this.y;
        };
        FileParser.prototype.getWord = function () {
            if (this.isEOF()) {
                this.reportError('a word expected, but EOF');
            }
            if (this.content[this.y].length <= this.x) {
                this.reportError('a word expected, but newline');
            }
            var word = this.content[this.y][this.x];
            this.x += 1;
            return word;
        };
        FileParser.prototype.getInt = function () {
            var word = this.getWord();
            if (!word.match(new RegExp('^[-+]?[0-9]+$'))) {
                this.reportError("a number expected, but word " + JSON.stringify(this.content[this.y][this.x]));
            }
            return parseInt(word);
        };
        FileParser.prototype.getNewline = function () {
            if (this.isEOF()) {
                this.reportError('newline expected, but EOF');
            }
            this.x = 0;
            this.y += 1;
        };
        FileParser.prototype.unwind = function () {
            if (this.x == 0) {
                this.y -= 1;
                this.x = this.content[this.y].length - 1;
            } else {
                this.x -= 1;
            }
        };
        FileParser.prototype.reportError = function (msg) {
            msg = this.filename + ": line " + (this.y + 1) + ": " + msg;
            alert(msg);
            throw new Error(msg);
        };
        return FileParser;
    }());
    framework.FileParser = FileParser;
    var FileSelector = /** @class */ (function () {
        function FileSelector() {
            var _this = this;
            this.inputFile = document.getElementById("inputFile");
            this.outputFile = document.getElementById("outputFile");
            this.reloadButton = document.getElementById("reloadButton");
            this.reloadFilesClosure = function () { _this.reloadFiles(); };
            this.inputFile.addEventListener('change', this.reloadFilesClosure);
            this.outputFile.addEventListener('change', this.reloadFilesClosure);
            this.reloadButton.addEventListener('click', this.reloadFilesClosure);
        }
        FileSelector.prototype.reloadFiles = function () {
            var _this = this;
            if (this.inputFile.files == null || this.inputFile.files.length == 0)
                return;
            loadFile(this.inputFile.files[0], function (inputContent) {
                if (_this.outputFile.files == null || _this.outputFile.files.length == 0)
                    return;
                loadFile(_this.outputFile.files[0], function (outputContent) {
                    _this.inputFile.removeEventListener('change', _this.reloadFilesClosure);
                    _this.outputFile.removeEventListener('change', _this.reloadFilesClosure);
                    _this.reloadButton.classList.remove('disabled');
                    if (_this.callback !== undefined) {
                        _this.callback(inputContent, outputContent);
                    }
                });
            });
        };
        return FileSelector;
    }());
    framework.FileSelector = FileSelector;
    var loadFile = function (file, callback) {
        var reader = new FileReader();
        reader.readAsText(file);
        reader.onloadend = function () {
            callback(reader.result);
        };
    };
})(framework || (framework = {}));

var visualizer;
(function (visualizer) {
    var InputFile = /** @class */ (function () {
        function InputFile(content) {
            var parser = new framework.FileParser('<input-file>', content.trim());
            parser.getWord();
            this.M = parser.getInt();
            this.I = parser.getInt();
            this.R = parser.getInt();
            this.BL = parser.getInt();
            this.CL = parser.getInt();
            parser.getNewline();

            this.boms = [];
            for (let i = 0; i < this.BL; ++i) {
                parser.getWord();
                var _i = parser.getInt();
                var m = parser.getInt();
                var s = parser.getInt();

                if (this.boms[_i] == undefined) {
                    this.boms[_i] = [];
                }
                this.boms[_i][m] = {}
                this.boms[_i][m] = s;

                parser.getNewline();
            }

            this.combi = [];
            for (let m = 0; m < this.M; ++m) {
                this.combi[m] = {};
                this.combi[m] = [];
                for (let ipre = 0; ipre < this.I; ++ipre) {
                    this.combi[m][ipre] = {};
                    this.combi[m][ipre] = [];
                }
            }
            for (let i = 0; i < this.CL; ++i) {
                parser.getWord();
                let m = parser.getInt();
                let ipre = parser.getInt();
                let inext = parser.getInt();
                let t = parser.getInt();
                this.combi[m][ipre][inext] = t;
                parser.getNewline();
            }

            this.orders = [];
            for (let i = 0; i < this.R; ++i) {
                parser.getWord();
                let r = parser.getInt();
                let _i = parser.getInt();
                let e = parser.getInt();
                let d = parser.getInt();
                let q = parser.getInt();
                let pr = parser.getInt();
                let a = parser.getInt();
                this.orders[r] = {};
                this.orders[r].i = _i;
                this.orders[r].e = e;
                this.orders[r].d = d;
                this.orders[r].q = q;
                this.orders[r].pr = pr;
                this.orders[r].a = a;
                parser.getNewline();
            }
            if (!parser.isEOF())
                parser.reportError("Too long file.");

        }
        return InputFile;
    }());

    var OutputFile = /** @class */ (function () {
        function OutputFile(content, inputFile) {
            /* ファイル読み込み */
            var parser = new framework.FileParser('<output-file>', content.trim());
            this.operations = [];
            for (let i = 0; i < inputFile.R; ++i) {
                let r = parser.getInt();
                let m = parser.getInt();
                let t1 = parser.getInt();
                let t2 = parser.getInt();
                let t3 = parser.getInt();

                if (this.operations[r] == undefined) {
                    this.operations[r] = {};
                }
                this.operations[r].r = r;
                this.operations[r].m = m;
                this.operations[r].t1 = t1;
                this.operations[r].t2 = t2;
                this.operations[r].t3 = t3;
                /* この変数は、表示するかどうかを表す。（対応するBOMが存在しなかった場合に表示させないようにするため） */
                this.operations[r].valid = true;
                parser.getNewline();
            }

            /* ファイル読み込み終了 */
            if (!parser.isEOF())
                parser.reportError("Too long file.");

            /* 段取り時間開始日時順にソート(後で戻す) */
            for (let r = 0; r < inputFile.R; ++r) {
                this.operations.sort(function (a, b) {
                    if (a.t1 > b.t1) {
                        return 1;
                    } else {
                        return -1;
                    }
                });
            }

            /* 正しい製造時間を計算 */
            var mToPreviousT3 = [];
            var mToPreviousI = [];
            var rpToTime = [];

            for (let r = 0; r < inputFile.R; ++r) {
                let operation = this.operations[r];
                let order = inputFile.orders[operation.r];
                let i = order.i;
                let m = operation.m

                rpToTime[operation.r] = {};
                rpToTime[operation.r] = [];

                /* 対応するBOMがある場合のみ計算する */
                if (inputFile.boms[i][m] != undefined) {
                    if (mToPreviousT3[m] == undefined) mToPreviousT3[m] = 0;

                    let t;
                    if (mToPreviousI[m] == undefined) {
                        t = 0;
                    } else if (inputFile.combi[m][mToPreviousI[m]][i] != undefined) {
                        t = inputFile.combi[m][mToPreviousI[m]][i];
                    } else {
                        t = 0;
                    }
                    /* 段取り時間を計算 (ここでは、前詰めで割付けずにt1に従う) */
                    let t1 = Math.max(mToPreviousT3[m], order.e, operation.t1);
                    var needResourceAmount = t;
                    let t2 = t1 + needResourceAmount;

                    /* 製造時間を計算 */
                    var needResourceAmount = order.q * inputFile.boms[i][m];
                    let t3 = t2 + needResourceAmount;

                    rpToTime[operation.r].valid = true;
                    rpToTime[operation.r].t1 = t1;
                    rpToTime[operation.r].t2 = t2;
                    rpToTime[operation.r].t3 = t3;
                    mToPreviousI[m] = i;
                    mToPreviousT3[m] = t3;
                } else {
                    rpToTime[operation.r].valid = false;
                    rpToTime[operation.r].t1 = 0;
                    rpToTime[operation.r].t2 = 0;
                    rpToTime[operation.r].t3 = 0;
                }
            }

            /* オーダー番号順にソート */
            for (let r = 0; r < inputFile.R; ++r) {
                this.operations.sort(function (a, b) {
                    if (a.r > b.r) {
                        return 1;
                    } else {
                        return -1;
                    }
                });
            }

            /* エラーチェック */
            var judge = false;
            for (let r = 0; r < inputFile.R; ++r) {
                let str = "\nオーダ番号: " + r;
                if (rpToTime[r].valid) {
                    if (rpToTime[r].t1 > this.operations[r].t1) {
                        alert("製造開始時刻違反" + str);
                        judge = true;
                    }
                    if (rpToTime[r].t2 - rpToTime[r].t1 != this.operations[r].t2 - this.operations[r].t1) {
                        alert("段取り時間間違い" + str);
                        judge = true;
                    }
                    if (rpToTime[r].t3 - rpToTime[r].t2 != this.operations[r].t3 - this.operations[r].t2) {
                        alert("製造時間間違い" + str);
                        judge = true;
                    }
                } else {
                    alert("BOM違反" + str);
                    judge = true;
                }
                if (judge) break;
            }

            if (judge) {
                if (confirm("正しい製造計画に変更しますか?")) {
                    for (let r = 0; r < inputFile.R; ++r) {
                        this.operations[r].t1 = rpToTime[r].t1;
                        this.operations[r].t2 = rpToTime[r].t2;
                        this.operations[r].t3 = rpToTime[r].t3;
                        this.operations[r].vaild = rpToTime[r].vaild;
                    }
                }
            }

            /* 最大日数計算 */
            this.max_t = 0;
            for (let r = 0; r < inputFile.R; ++r) {
                this.max_t = Math.max(this.max_t, this.operations[r].t3, inputFile.orders[r].d);
            }
        }
        return OutputFile;
    }());

    var TesterFrame = /** @class */ (function () {
        function TesterFrame(input, output) {
            this.input = input;
            this.output = output;

            this.grossP = 10000000000;
            this.dcost = 0.0;
            this.scost = 0.0;
            this.profit = 0.0;

            for (let r = 0; r < input.R; ++r) {
                let d = input.orders[r].d;
                let a = input.orders[r].a;
                let pr = input.orders[r].pr;
                let t1 = output.operations[r].t1;
                let t2 = output.operations[r].t2;
                let t3 = output.operations[r].t3;
                /* 粗利計算 */
                this.grossP += pr;
                /* コスト計算 */
                let delay = Math.max(0, t3 - d);
                let pe1 = Math.ceil(pr * delay / a);
                let pe2 = t2 - t1;
                this.dcost += pe1;
                this.scost += pe2;
            }
            this.profit = this.grossP - this.dcost - this.scost;
        }
        return TesterFrame;
    }());

    var Tester = /** @class */ (function () {
        function Tester(inputContent, outputContent) {
            var input = new InputFile(inputContent);
            var output = new OutputFile(outputContent, input);
            this.frame = new TesterFrame(input, output);
        }
        return Tester;
    }());

    var Visualizer = /** @class */ (function () {
        function Visualizer() {
            this.grossProfitInput = document.getElementById('grossP');
            this.dcostSumInput = document.getElementById('d-cost');
            this.scostSumInput = document.getElementById('s-cost');
            this.totalProfitInput = document.getElementById('score');
            this.tooltipDiv = document.getElementById("tooltip");
            this.drawingElement = document.getElementById('drawing');
            this.zoominButton = document.getElementById('zoomin');
            this.zoomoutButton = document.getElementById('zoomout');
            this.resizeButton = document.getElementById('resize');
            this.padding = 14;
            this.width = this.drawingElement.clientWidth - (2 * this.padding);
            this.height = this.drawingElement.clientHeight - (2 * this.padding);
            this.svg = SVG('drawing').size(this.width, this.height);
            this.defaultViewbox = this.svg.viewbox();
        }
        Visualizer.prototype.drawChart = function (frame) {
            var _this = this;
            this.svg.remove();
            this.width = this.drawingElement.clientWidth - (2 * this.padding);
            this.height = this.drawingElement.clientHeight - (2 * this.padding);
            this.svg = SVG('drawing').size(this.width, this.height);
            var input = frame.input;
            var output = frame.output;

            /* レイアウトに必要な数値計算 */
            var rowHeight = Math.min(130, this.height / input.M);
            var chartHeight = rowHeight * 0.8;
            var machineDetailHeight = rowHeight * 0.075;
            var verticalLabelWidth = this.width * 0.04;
            var chartpadding = chartHeight * 0.2;
            var verticalTextSize = verticalLabelWidth * 0.2;
            var chartWidth = this.width - verticalLabelWidth - verticalTextSize * 2;
            var verticalbarinterval = Math.round(Math.pow(10, Math.floor(Math.log10(output.max_t)) - 1));
            if (Math.log10(output.max_t) % 1 > 0.3010)
                verticalbarinterval = Math.round(Math.pow(10, Math.floor(Math.log10(output.max_t)) - 1)) * 5;

            /* 各設備の大枠を描写 */
            for (let m = 0; m < input.M; ++m) {
                let mathineDetailText = "設備m: " + m;
                /* 設備のデータテキストを配置 */
                this.svg.text(mathineDetailText).font({
                    size: machineDetailHeight * 1.2,
                    family: 'Menlo, sans-serif',
                    fill: '#000'
                }).move(verticalLabelWidth + (machineDetailHeight * 0.9) * 1, m * rowHeight + rowHeight * 0.08);
                /* 日付区切り等の線を配置 */
                let bx1 = verticalLabelWidth + verticalTextSize;
                let bx2 = this.width;
                let by = (m + 1) * rowHeight - chartpadding;
                this.svg.line(bx1, by, bx2, by).stroke({ color: '#000', width: 0.5 });
                for (let t = 0; t <= output.max_t; t += verticalbarinterval) {
                    let x = (chartWidth / output.max_t) * t + verticalLabelWidth + verticalTextSize;
                    let y1 = m * rowHeight + machineDetailHeight + chartpadding;
                    let y2 = (m + 1) * rowHeight - chartpadding;
                    this.svg.line(x, y1, x, y2).stroke({ color: '#000', width: 0.5 });
                    if (output.max_t <= 30 * verticalbarinterval) {
                        this.svg.text(t + "").font({
                            size: verticalTextSize,
                            family: 'Menlo, sans-serif',
                            anchor: 'end',
                            fill: '#000'
                        }).move(x, y1 - verticalTextSize);
                    } else if (output.max_t / verticalbarinterval <= 99) {
                        this.svg.text(t + "").font({
                            size: verticalTextSize * 0.8,
                            family: 'Menlo, sans-serif',
                            anchor: 'end',
                            fill: '#000'
                        }).move(x, y1 - verticalTextSize * 0.8);
                    } else {
                        this.svg.text(t + "").font({
                            size: verticalTextSize * 0.4,
                            family: 'Menlo, sans-serif',
                            anchor: 'end',
                            fill: '#000'
                        }).move(x, y1 - verticalTextSize * 0.4);
                    }
                }
            }

            /* オーダごとに作業を描写 */
            var rects = [];
            for (let r = 0; r < input.R; ++r) {
                if (!output.operations[r].valid) continue;
                let i = input.orders[r].i;
                let dandoriColor = '#548235';
                let itemColor = this.color(i);
                rects[r] = [];
                let m = output.operations[r].m;
                let t1 = output.operations[r].t1;
                let t2 = output.operations[r].t2;
                let t3 = output.operations[r].t3;
                /* 段取り時間を描写 */
                while (t2 > t1) {
                    var nPeriod = parseInt((t1 + verticalbarinterval) / verticalbarinterval);
                    nPeriod = Math.max(1, nPeriod);
                    var height = (chartHeight - 2 * chartpadding) * 0.8;
                    var width = Math.min(t2 - t1, nPeriod * verticalbarinterval - t1) * chartWidth / output.max_t;
                    var x = verticalLabelWidth + verticalTextSize + t1 * chartWidth / output.max_t;
                    var y = (m + 1) * rowHeight - chartpadding - height;
                    rects[r].push(this.svg.rect(width, height).fill({ color: dandoriColor, opacity: 0.8 }).attr({ stroke: '#fff', 'stroke-width': 5, 'stroke-opacity': 0 }).move(x, y));
                    t1 = Math.min(t2, nPeriod * verticalbarinterval);
                }
                /* 工程時間を描写 */
                while (t3 > t2) {
                    var nPeriod = parseInt((t2 + verticalbarinterval) / verticalbarinterval);
                    nPeriod = Math.max(1, nPeriod);
                    var height = (chartHeight - 2 * chartpadding);
                    var width = Math.min(t3 - t2, nPeriod * verticalbarinterval - t2) * chartWidth / output.max_t;
                    var x = verticalLabelWidth + verticalTextSize + t2 * chartWidth / output.max_t;
                    var y = (m + 1) * rowHeight - chartpadding - height;
                    rects[r].push(this.svg.rect(width, height).fill({ color: itemColor, opacity: 0.8 }).attr({ stroke: '#fff', 'stroke-width': 5, 'stroke-opacity': 0 }).move(x, y));
                    t2 = Math.min(t3, nPeriod * verticalbarinterval);
                }
            }

            /* 納期遅れオーダを探す */
            for (let r = 0; r < input.R; ++r) {
                if (input.orders[r].d < output.operations[r].t3) {
                    let m = output.operations[r].m;
                    let t1 = output.operations[r].t1;
                    let t3 = output.operations[r].t3;
                    let x = verticalLabelWidth + verticalTextSize + ((t1 + t3) / 2) * chartWidth / output.max_t;
                    let y = (m + 1) * rowHeight + chartpadding * 0.85 - chartHeight;
                    this.svg.text("!").font({
                        size: 1.8 * verticalTextSize,
                        family: 'Menlo, sans-serif',
                        fill: '#f00'
                    }).move(x - verticalTextSize / 2.1, y - verticalTextSize * 1.8);
                }
            }

            /* マウスホバーで線とオーダ情報を表示 */
            var rectlines = [];
            var edlines = [];
            var edpolys = [];
            var edtexts = [];
            var strs = [];
            for (let r = 0; r < input.R; ++r) {
                if (!output.operations[r].valid) continue;
                rectlines[r] = [];
                edlines[r] = [];
                edpolys[r] = [];
                edtexts[r] = [];
                strs[r] = [];
                let m = output.operations[r].m;

                let d = input.orders[r].d;
                let a = input.orders[r].a;
                let pr = input.orders[r].pr;
                let t1 = output.operations[r].t1;
                let t2 = output.operations[r].t2;
                let t3 = output.operations[r].t3;
                /* コスト計算 */
                let delay = Math.max(0, t3 - d);
                let pe1 = Math.ceil(pr * delay / a);
                let pe2 = t2 - t1;

                /* オーダの情報をまとめた文字列を作る */
                let str = "オーダ番号: " + r + "<br>" +
                    "品目番号: " + input.orders[r].i + "<br>" +
                    "製造数量: " + input.orders[r].q + "<br>" +
                    "最早開始時刻: " + input.orders[r].e + "秒<br>" +
                    "納期時刻: " + input.orders[r].d + "秒<br>" +
                    "納期遅れ許容時間: " + input.orders[r].a + "秒<br>" +
                    "現在工程の段取り開始時刻: " + output.operations[r].t1 + "秒<br>" +
                    "現在工程の製造開始時刻: " + output.operations[r].t2 + "秒<br>" +
                    "現在工程の製造終了時刻: " + output.operations[r].t3 + "秒<br>" +
                    "粗利金額: " + input.orders[r].pr + "<br>" +
                    "納期遅れペナルティ額: " + pe1 + "<br>" +
                    "段取り時間ペナルティ額: " + pe2 + "<br>" +
                    "粗利−ペナルティ: " + (input.orders[r].pr - pe1 - pe2) + "<br>";

                strs[r] = str;
                let S = rects[r].length;
                for (let s = 0; s < S; ++s) {
                    /* オーダを紐付ける線の配列を作る */
                    let width = rects[r][s].width();
                    let height = rects[r][s].height();
                    let x = rects[r][s].x();
                    let y = rects[r][s].y();
                    if (s == 0) {
                        rectlines[r].push(this.svg.line(x, y, x, y + height).stroke({ color: '#000', width: 0 }));
                        rectlines[r].push(this.svg.line(x, y, x + width, y).stroke({ color: '#000', width: 0 }));
                        rectlines[r].push(this.svg.line(x, y + height, x + width, y + height).stroke({ color: '#000', width: 0 }));
                        if (s + 1 == S) {
                            rectlines[r].push(this.svg.line(x + width, y, x + width, y + height).stroke({ color: '#000', width: 0 }));
                        } else {
                            let nxtX = rects[r][s + 1].x();
                            let nxtY = rects[r][s + 1].y();
                            rectlines[r].push(this.svg.line(x + width, y, nxtX, nxtY).stroke({ color: '#000', width: 0 }));
                        }
                    } else if (s + 1 == S) {
                        rectlines[r].push(this.svg.line(x, y, x + width, y).stroke({ color: '#000', width: 0 }));
                        rectlines[r].push(this.svg.line(x, y + height, x + width, y + height).stroke({ color: '#000', width: 0 }));
                        rectlines[r].push(this.svg.line(x + width, y, x + width, y + height).stroke({ color: '#000', width: 0 }));
                    } else {
                        rectlines[r].push(this.svg.line(x, y, x + width, y).stroke({ color: '#000', width: 0 }));
                        rectlines[r].push(this.svg.line(x, y + height, x + width, y + height).stroke({ color: '#000', width: 0 }));
                        let nxtX = rects[r][s + 1].x();
                        let nxtY = rects[r][s + 1].y();
                        rectlines[r].push(this.svg.line(x + width, y, nxtX, nxtY).stroke({ color: '#000', width: 0 }));
                    }
                }
                var triangleMargin = 4;
                var triangleWidth = 14;
                var triangleHeight = 13;
                var labelVerticalMargin = 20;
                var labelLeft = 0;
                
                /* 最早開始日時の線を作る */
                var x = verticalLabelWidth + verticalTextSize + (chartWidth / output.max_t) * input.orders[r].e;
                var y1 = m * rowHeight + machineDetailHeight + chartpadding;
                var y2 = (m + 1) * rowHeight - chartpadding;
                var triangle = [[x, y2 + triangleMargin],[x - triangleWidth / 2, y2 + triangleMargin + triangleHeight],[x + triangleWidth / 2, y2 + triangleMargin + triangleHeight]];
                var label = 'e=' + input.orders[r].e;
                edlines[r].push(this.svg.line(x, y1, x, y2).stroke({ color: '#00f', width: 0 }));
                edpolys[r].push(this.svg.polygon().plot(triangle).fill('none'));
                edtexts[r].push(this.svg.text(label).font({
                    size: machineDetailHeight * 0.9,
                    family: 'Menlo, sans-serif',
                    anchor: 'middle',
                    fill: 'none'
                }).move(x, y2 + labelVerticalMargin));
                labelLeft = x + 40;

                /* 納期の線を作る */
                var x = verticalLabelWidth + verticalTextSize + (chartWidth / output.max_t) * input.orders[r].d;
                var y1 = m * rowHeight + machineDetailHeight + chartpadding;
                var y2 = (m + 1) * rowHeight - chartpadding;
                var triangle = [[x, y2 + triangleMargin],[x - triangleWidth / 2, y2 + triangleMargin + triangleHeight],[x + triangleWidth / 2, y2 + triangleMargin + triangleHeight]];
                var label = 'd=' + input.orders[r].d;
                edlines[r].push(this.svg.line(x, y1, x, y2).stroke({ color: '#f80', width: 0 }));
                edpolys[r].push(this.svg.polygon().plot(triangle).fill('none'));
                edtexts[r].push(this.svg.text(label).font({
                    size: machineDetailHeight * 0.9,
                    family: 'Menlo, sans-serif',
                    anchor: 'middle',
                    fill: 'none'
                }).move(Math.max(x, labelLeft), y2 + labelVerticalMargin));
                labelLeft = Math.max(x, labelLeft) + 50;

                /* 納期遅れ許容の線を作る */
                var x = verticalLabelWidth + verticalTextSize + (chartWidth / output.max_t) * (input.orders[r].d + input.orders[r].a);
                var y1 = m * rowHeight + machineDetailHeight + chartpadding;
                var y2 = (m + 1) * rowHeight - chartpadding;
                var triangle = [[x, y2 + triangleMargin],[x - triangleWidth / 2, y2 + triangleMargin + triangleHeight],[x + triangleWidth / 2, y2 + triangleMargin + triangleHeight]];
                var label = 'd+a=' + (input.orders[r].d + input.orders[r].a);
                if (input.orders[r].a) {
                    edlines[r].push(this.svg.line(x, y1, x, y2).stroke({ color: '#f00', width: 0 }));
                    edpolys[r].push(this.svg.polygon().plot(triangle).fill('none'));
                    edtexts[r].push(this.svg.text(label).font({
                        size: machineDetailHeight * 0.9,
                        family: 'Menlo, sans-serif',
                        anchor: 'middle',
                        fill: 'none'
                    }).move(Math.max(x, labelLeft), y2 + labelVerticalMargin));
                }
            }

            /* 作業順変更用 */
            /* 作業順を変更して割付けエラーがあったときの再割付けにバグがあるっぽい？
            let cover = this.svg.rect(this.width, this.height).fill({ color: '#FFF', opacity: 0.5 }).hide();
            let timebarsets = [];
            let timebars = [];
            let targetOrder = -1;
            for (let m = 0; m < input.M; ++m) {
                timebarsets[m] = new Set();
                timebarsets[m].add(0);
            }
            for (let r = 0; r < input.R; ++r) {
                let operation = output.operations[r];
                let order = input.orders[r];
                timebarsets[operation.m].add(operation.t1);
                timebarsets[operation.m].add(operation.t3);
                timebarsets[operation.m].add(order.e);
                timebarsets[operation.m].add(order.d);
            }
            for (let m = 0; m < input.M; ++m) {
                for (let time of timebarsets[m]) {
                    let x = verticalLabelWidth + verticalTextSize + time * chartWidth / output.max_t;
                    let y1 = m * rowHeight + machineDetailHeight + chartpadding;
                    let y2 = (m + 1) * rowHeight - chartpadding;
                    let _this = this;
                    let bar = this.svg.line(x, y1, x, y2)
                        .stroke({ color: '#F00', width: 2 })
                        .mouseover(function (params) {
                            bar.stroke({ color: '#F00', width: 4 });
                        })
                        .mouseout(function (params) {
                            bar.stroke({ color: '#F00', width: 2 });
                        })
                        .click(function (params) {
                            // outputを更新
                            let prevOperations = $.extend(true, {}, output.operations);
                            output.operations[targetOrder].m = m;
                            for (let r = 0; r < input.R; ++r) {
                                if (output.operations[r].m == m && output.operations[r].t1 == time) {
                                    output.operations[r].t1++;
                                }
                            }
                            output.operations[targetOrder].t1 = time;

                            // 段取り時間開始日時順にソート(後で戻す)
                            for (let r = 0; r < input.R; ++r) {
                                output.operations.sort(function (a, b) {
                                    if (a.t1 > b.t1) {
                                        return 1;
                                    } else {
                                        return -1;
                                    }
                                });
                            }

                            // 正しい製造時間を計算
                            var mToPreviousT3 = [];
                            var mToPreviousI = [];
                            var rpToTime = [];

                            for (let r = 0; r < input.R; ++r) {
                                let operation = output.operations[r];
                                let order = input.orders[operation.r];
                                let i = order.i;
                                let m = operation.m

                                rpToTime[operation.r] = {};
                                rpToTime[operation.r] = [];

                                // 対応するBOMがある場合のみ計算する
                                if (input.boms[i][m] != undefined) {
                                    if (mToPreviousT3[m] == undefined) mToPreviousT3[m] = 0;

                                    let t;
                                    if (mToPreviousI[m] == undefined) {
                                        t = 0;
                                    } else if (input.combi[m][mToPreviousI[m]][i] != undefined) {
                                        t = input.combi[m][mToPreviousI[m]][i];
                                    } else {
                                        t = 0;
                                    }
                                    // 段取り時間を計算 (ここでは、前詰めで割付けずにt1に従う)
                                    let t1 = Math.max(mToPreviousT3[m], order.e, operation.t1);
                                    var needResourceAmount = t;
                                    let t2 = t1 + needResourceAmount;

                                    // 製造時間を計算
                                    var needResourceAmount = order.q * input.boms[i][m];
                                    let t3 = t2 + needResourceAmount;

                                    rpToTime[operation.r].valid = true;
                                    rpToTime[operation.r].t1 = t1;
                                    rpToTime[operation.r].t2 = t2;
                                    rpToTime[operation.r].t3 = t3;
                                    mToPreviousI[m] = i;
                                    mToPreviousT3[m] = t3;
                                } else {
                                    rpToTime[operation.r].valid = false;
                                    rpToTime[operation.r].t1 = 0;
                                    rpToTime[operation.r].t2 = 0;
                                    rpToTime[operation.r].t3 = 0;
                                }
                            }

                            // オーダー番号順にソート
                            for (let r = 0; r < input.R; ++r) {
                                output.operations.sort(function (a, b) {
                                    if (a.r > b.r) {
                                        return 1;
                                    } else {
                                        return -1;
                                    }
                                });
                            }

                            // エラーチェック
                            var judge = false;
                            for (let r = 0; r < input.R; ++r) {
                                let str = "\nオーダ番号: " + r;
                                if (rpToTime[r].valid) {
                                    if (rpToTime[r].t1 > output.operations[r].t1) {
                                        alert("製造開始時刻違反" + str);
                                        judge = true;
                                    }
                                } else {
                                    alert("BOM違反" + str);
                                    judge = true;
                                }
                                if (judge) break;
                            }

                            for (let r = 0; r < input.R; ++r) {
                                output.operations[r].t1 = rpToTime[r].t1;
                                output.operations[r].t2 = rpToTime[r].t2;
                                output.operations[r].t3 = rpToTime[r].t3;
                                output.operations[r].vaild = rpToTime[r].vaild;
                            }

                            // 最大日数計算
                            output.max_t = 0;
                            for (let r = 0; r < input.R; ++r) {
                                output.max_t = Math.max(output.max_t, output.operations[r].t3, input.orders[r].d);
                            }
                            if (judge) {
                                if (!confirm("正しい製造計画に変更しますか?")) {
                                    for (let r = 0; r < input.R; ++r) {
                                        output.operations[r].m = prevOperations[r].m;
                                        output.operations[r].r = prevOperations[r].r;
                                        output.operations[r].t1 = prevOperations[r].t1;
                                        output.operations[r].t2 = prevOperations[r].t2;
                                        output.operations[r].t3 = prevOperations[r].t3;
                                        output.operations[r].vaild = prevOperations[r].vaild;
                                    }
                                }
                            }
                            // 割り付けなおし
                            frame = new TesterFrame(input, output);
                            _this.draw(frame);
                        })
                        .hide()
                    timebars.push(bar);
                }
            }
            */

            /* マウスホバーしたときの表示を登録する */
            for (let r = 0; r < input.R; ++r) {
                if (!output.operations[r].valid) continue;
                let i = input.orders[r].i;
                for (let s = 0; s < rects[r].length; ++s) {
                    let str = strs[r];
                    for (let i = 0; i < rectlines[r].length; ++i) {
                        rectlines[r][i].stroke({ width: 1 });
                    }
                    rects[r][s].mouseover(function (evt) {
                        _this.rectsmouseOn(rectlines[r], edlines[r], edpolys[r], edtexts[r], str, evt);
                    })
                    rects[r][s].mouseout(function (evt) {
                        _this.rectsmouseOff(rectlines[r], edlines[r], edpolys[r], edtexts[r], evt);
                    })
                    rects[r][s].dblclick(function (evt) {
                        cover.show();
                        targetOrder = r;
                        for (let i = 0; i < timebars.length; ++i) {
                            timebars[i].show();
                        }
                    })
                }
            }
        }
        Visualizer.prototype.setScore = function (frame) {
            this.grossProfitInput.value = String(frame.grossP).replace(/(\d)(?=(\d\d\d)+(?!\d))/g, '$1,');
            this.dcostSumInput.value = String(frame.dcost).replace(/(\d)(?=(\d\d\d)+(?!\d))/g, '$1,');
            this.scostSumInput.value = String(frame.scost).replace(/(\d)(?=(\d\d\d)+(?!\d))/g, '$1,');
            this.totalProfitInput.value = String(frame.profit).replace(/(\d)(?=(\d\d\d)+(?!\d))/g, '$1,');
        }
        Visualizer.prototype.setZoomAction = function () {
            // this.reloadButton.addEventListener('click', this.reloadFilesClosure);
            var _this = this;
            this.zoominButton.addEventListener('click', function () {
                var viewbox = _this.svg.viewbox();
                _this.svg.viewbox(viewbox.x, viewbox.y, viewbox.width * 0.95, viewbox.height * 0.95);
            });
            this.zoomoutButton.addEventListener('click', function () {
                var viewbox = _this.svg.viewbox();
                _this.svg.viewbox(viewbox.x, viewbox.y, viewbox.width * 1.05, viewbox.height * 1.05);
            });
            this.resizeButton.addEventListener('click', function () {
                _this.svg.viewbox(_this.defaultViewbox);
            });
            this.drawingElement.addEventListener('mousedown', function (evt) {
                var x = evt.clientX;
                var y = evt.clientY;
                var viewbox = _this.svg.viewbox();
                var mmove = function (evt) {
                    var nx = evt.clientX;
                    var ny = evt.clientY;
                    _this.svg.viewbox(viewbox.x - (nx - x), viewbox.y - (ny - y), viewbox.width, viewbox.height);
                }
                var mup = function () {
                    _this.drawingElement.removeEventListener('mousemove', mmove);
                    _this.drawingElement.removeEventListener('mouseup', mup);
                }
                _this.drawingElement.addEventListener('mousemove', mmove);
                _this.drawingElement.addEventListener('mouseup', mup);
            });
            this.zoominButton.classList.remove('disabled');
            this.zoomoutButton.classList.remove('disabled');
            this.resizeButton.classList.remove('disabled');
        }
        Visualizer.prototype.draw = function (frame) {
            this.setScore(frame);
            this.drawChart(frame);
            this.setZoomAction();
        }
        Visualizer.prototype.rectsmouseOn = function (rectlines, edlines, edpolys, edtexts, str, evt) {
            for (let i = 0; i < rectlines.length; ++i) {
                rectlines[i].stroke({ width: 2 });
            }
            for (let i = 0; i < edlines.length; ++i) {
                edlines[i].stroke({ width: 2 });
            }
            for (let i = 0; i < edpolys.length; ++i) {
                switch(i % 3) {
                    case 0:
                        edpolys[i].fill('#00f');
                        break;
                    case 1:
                        edpolys[i].fill('#f80');
                        break;
                    case 2:
                        edpolys[i].fill('#f00');
                        break;
                }
            }
            for (let i = 0; i < edtexts.length; ++i) {
                edtexts[i].font({
                        fill: '#000'
                    });
            }
            this.tooltipDiv.innerHTML = str;
            this.tooltipDiv.style.display = "block";
            this.tooltipDiv.style.left = (evt.clientX + 50) + 'px';
            this.tooltipDiv.style.top = (evt.clientY - 200) + 'px';


        }
        Visualizer.prototype.rectsmouseOff = function (rectlines, edlines, edpolys, edtexts, evt) {
            for (let i = 0; i < rectlines.length; ++i) {
                rectlines[i].stroke({ width: 1 });
            }
            for (let i = 0; i < edlines.length; ++i) {
                edlines[i].stroke({ width: 0 });
            }
            for (let i = 0; i < edpolys.length; ++i) {
                edpolys[i].fill('none');
            }
            for (let i = 0; i < edtexts.length; ++i) {
                edtexts[i].font({
                        fill: 'none'
                    });
            }
            this.tooltipDiv.style.display = "none";
        }
        Visualizer.prototype.linemouseOn = function (line, str, evt) {
            line.stroke({ color: '#000', opacity: 1 });
            this.tooltipDiv.innerHTML = str;
            this.tooltipDiv.style.display = "block";
            this.tooltipDiv.style.left = (evt.clientX + 10) + 'px';
            this.tooltipDiv.style.top = (evt.clientY - 10) + 'px';

        }
        Visualizer.prototype.linemouseOff = function (line, evt) {
            line.stroke({ color: '#fff', opacity: 0 });
            this.tooltipDiv.style.display = "none";
        }
        Visualizer.prototype.hideTooltip = function (evt) {
            this.tooltipDiv.style.display = "none";
        };
        Visualizer.prototype.color = function (r) {
            var colors = [
                '#ff4500', '#f0e68c', '#66cdaa', '#4169e1', '#eed4d4',
                '#696969', '#c71585', '#ff8c00', '#7cfc00', '#000080',
                '#ffd2b3', '#800080', '#8b4513', '#bdb76b', '#bdb76b',
                '#00ffff', '#000000', '#7b68ee', '#d2691e', '#e6e6fa'
            ];
            return colors[r % colors.length];
        };
        return Visualizer;
    }());

    var App = /** @class */ (function () {
        function App() {
            var _this = this;
            this.visualizer = new Visualizer();
            this.loader = new framework.FileSelector();
            this.loader.callback = function (inputContent, outputContent) {
                _this.tester = new Tester(inputContent, outputContent);
                _this.visualizer.draw(_this.tester.frame);
            };
        }
        return App;
    }());
    visualizer.App = App;
})(visualizer || (visualizer = {}));

window.onload = function () {
    new visualizer.App();
};
