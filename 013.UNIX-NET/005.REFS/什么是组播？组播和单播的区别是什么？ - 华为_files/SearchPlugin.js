//最近浏览默认为空
var recentProductNull = false;
var searchPlugin = {
    searchPluginConfig: {
        selector: "",
        lang: '',
        width: null,
        widthSelector: null,
        appendTo: null,
        searchPlaceholder: "",
        isLogin: false,
        whichPage: ''
    },
    /**
     * 初始化控件（供外部使用）enterprise
     */

    init: function (config) {
        this.searchPluginConfig = $.extend(this.searchPluginConfig, config);
        _extAutocomplete();
        _initAutoComplete(config);
    },
    option: function (config) {
        this.searchPluginConfig = $.extend(this.searchPluginConfig, config);
    }
}

/**
 * 初始化控件（供自身使用）
 */
function _initAutoComplete() {
    var $input = $(searchPlugin.searchPluginConfig.selector);
    if ($input.length == 0) {
        return;
    }
    $input.catcomplete({
        appendTo: searchPlugin.searchPluginConfig.appendTo,
        minLength: 0,
        source: function (request, response) {
            var keyword = $.trim($input.val());

            var language = $('#lang').val();

            var isMobile = false;

            if ((navigator.userAgent.match(/(phone|pod|iPhone|iPod|ios|Android|BlackBerry|IEMobile|MQQBrowser|JUC|Fennec|wOSBrowser|BrowserNG|WebOS|Symbian|Windows Phone)/i))) {
                isMobile = true;
            }
            //联想不需要替换特殊字符
            //keyword = replaceSpecialCharacters(keyword);
            if (keyword.length == 0) {
            } else {
                _querysearchProduct(keyword, isMobile, language, response);//搜索的内容
            }
        },
        //控件中 选择路径跳转
        select: function (event, ui) {
            stopBubble(event);
            var keyword = $.trim(ui.item.value);
            keyword = replaceSpecialCharacters(keyword);
            //联想不需要替换特殊字符
            var whichPage = searchPlugin.searchPluginConfig.whichPage;
            var domain = $('#domain').val();
            if (whichPage == 'index' || whichPage == 'pcDetail' ||whichPage == 'pcDetailCarrier') {//从首页跳转
                var lang = $('#lang').val();
                //搜索后跳转
                sessionStorage.setItem('searchWord', keyword);
                sessionStorage.setItem('isExactSearch', 'true');
                if(domain=='1'){
                    location.href = $("#contextPath").val() + '/encyclopedia/' + lang + '/carrier/detail?action=queryEntityDetail&keyword=' + keyword;
                }else{
                    location.href = $("#contextPath").val() + '/encyclopedia/' + lang + '/detail?action=queryEntityDetail&keyword=' + keyword;
                }

            }

        }
    });

}

/**
 * 产品联想（点击面包屑搜索框输入关键字查看联想结果）
 */
function _querysearchProduct(keyword, isMobile, language, response) {
    if ("" == keyword || keyword.length == 1) {
        $('#search-count #ui-id-1').hide();
    } else if (keyword.length >= 2) {
        //输入框内的内容进行适配
        var commonPath = $("#contextPath").val();
        var domain = $('#domain').val();
        $.ajax({
            contentType: 'application/json',
            type: "POST",
            data: JSON.stringify({lang:language,domain:domain,keyword:keyword}),
            url: commonPath + '/baike/queryMatchedEntityList',
            dataType: "json",
            async: false,
            success: function (res) {
                var data = res.data;
                var whichPage = searchPlugin.searchPluginConfig.whichPage;
                var autoBefore = '';
                var autoAfter = "";

                var newList = [];
                var matchedList = data ? data.matchedList : [];
                if (!matchedList || matchedList.length == 0) {
                    response(newList);
                    $("#home-warning").show();
                    $('#search-count').hide();
                    return false;
                }
                $("#home-warning").hide();
                $('#search-count').show();

                if (whichPage == 'index') {
                    pcIndex.isExist = true;
                } else if (whichPage == 'pcDetail') {
                    pcDetail.isExist = true;
                }else if(whichPage == 'pcDetailCarrier'){
                    pcDetailCarrier.isExist = true;
                }


                var max = 10;
                var len = matchedList.length;
                //只取前10个
                if (len > max) {
                    matchedList.splice(max);
                } else {
                    max = len;
                }
                for (var i = 0; i < max; i++) {
                    var item = matchedList[i].name;
                    var result = {
                        value: item,
                        NodeName: item,
                    };
                    newList.push(result);
                }
                response(newList);
                var inputSelect = $('.search-area .input-box');
                var uiidwidth = $(inputSelect).css('width');
                $("#search-count #ui-id-2").css("width", uiidwidth);
                $("#search-count #ui-id-1").css("width", uiidwidth);
            },
            error: function (err) {
            }
        });
    }
}

function filterDataList(data, lang) {
    var newList = [];
    if (data) {
        data.forEach(function (item, index) {
            if (lang == 'zh' && item.pathZh) {
                newList.push(item);
            }
            if (lang == 'en' && item.pathEn) {
                newList.push(item);
            }
        });
    }
    return newList;
}

/**
 * 初始化提示信息(搜索框点击效果)
 */
function _initSearchPlaceholder($input) {
    var searchPlaceholder = searchPlugin.searchPluginConfig.searchPlaceholder;
    $input.val(searchPlaceholder);

    $input.focus(function () {
        var keyword = $input.val();
        $input.catcomplete("search", keyword);
    });

    $input.blur(function () {
        var keyword = $input.val();
        if (keyword.length <= 0) {
            $input.val("");
        }
    });
}

//用于将联想框中结果与关键字相匹配的进行加粗处理
function highlightInfofinderKeyword(resultName, keyword) {
    if (!keyword || !resultName) {
        return resultName;
    }
    var splitLocation = resultName.lastIndexOf("»");
    if (splitLocation != -1) {
        // 仅获取最后一个节点的内容，以分隔符»分割开
        var resultName_prefix = resultName.substring(0, splitLocation);
        var resultNameList = resultName.substring(splitLocation, resultName.length);
        return resultName_prefix + resultNameList.replace(new RegExp("(?![^&;]+;)(?!<[^<>]*)("
            + keyword.replace(/([\^\$\(\)\[\]\{\}\*\.\+\?\|\\])/gi, "\\$1")
            + ")(?![^<>]*>)(?![^&;]+;)", "gi"), "<strong>$1</strong>");
    } else {
        return resultName.replace(new RegExp("(?![^&;]+;)(?!<[^<>]*)("
            + keyword.replace(/([\^\$\(\)\[\]\{\}\*\.\+\?\|\\])/gi, "\\$1")
            + ")(?![^<>]*>)(?![^&;]+;)", "gi"), "<strong>$1</strong>");
    }
}

/**
 * 重写_renderMenu方法，产品内容展示
 */
function _extAutocomplete() {
    var isLogin = $('#isLogin').val();

    $.widget("custom.catcomplete", $.ui.autocomplete, {
        _create: function () {
            this._super();
            this.widget().menu("option", "items", "> :not(.ui-autocomplete-category)");
        },
        _renderMenu: function (ul, items) {
            var that = this,
                currentCategory = "";
            var keyword = $.trim($(searchPlugin.searchPluginConfig.selector).val());

            if (keyword.length == 0) {
                //登录未输入关键字;用户最近浏览展示
                ul.removeClass('new-search-list1 new-search-list2 new-search-list3 new-search-list4');
                ul.append("<li class='ui-autocomplete-category' style='font-size:14px;padding-left:10px;padding-top: 7px;padding-bottom:6px'>" + i18nProperties_info.browsingHistory + "</li>");
                $.each(items, function (index, item) {
                    var NodeName = (item.NodeName);
                    var li;
                    li = that._renderItemData(ul, item);
                    li.attr("title", NodeName);

                    var $input = $(searchPlugin.searchPluginConfig.selector);
                    var keyword = $input.val();
                    li.children().html(checkXssHTML(NodeName));
                    li.children().css({"padding-left": "10px"});
                });
            } else {
                //产品联想展示
                var that = this,
                    currentCategory = "";
                ul.removeClass('new-search-list1 new-search-list2 new-search-list3 new-search-list4');
                $.each(items, function (index, item) {
                    var NodeName = (item.NodeName);
                    var li;
                    li = that._renderItemData(ul, item);
                    li.attr("title", NodeName);
                    var $input = $(searchPlugin.searchPluginConfig.selector);
                    var keyword = $input.val();
                    li.children().html(checkXssHTML(NodeName));
                    li.children().css({"padding-left": "10px"});
                });

            }
        },
        _resizeMenu: function () {
            var widthSelector = searchPlugin.searchPluginConfig.widthSelector;
            if (widthSelector) {
                if (typeof widthSelector == 'string' && $(widthSelector).length == 0) {
                    return;
                }
                this.menu.element.outerWidth($(widthSelector).outerWidth());
                return;
            }

            var width = searchPlugin.searchPluginConfig.width;
            if (width) {
                this.menu.element.outerWidth(width);
                return;
            }
        }
    });

}

function replaceSpecialCharacters(keyword) {
    var entityNameMappingWord = JSON.parse($('#entityNameMappingWord').val());
    $.each(entityNameMappingWord, function (i, v) {
        if (keyword.indexOf(i) > -1) {
            keyword = keyword.replace(i, v);
        }

    })
    return keyword
}

