/* Interactive Font Size Scaler for sip2json documentation */
(function() {
    function initFontSizeControl() {
        if (document.getElementById('font-size-controls')) return;

        var header = document.querySelector('.md-header__inner');
        if (!header) return;

        var container = document.createElement('div');
        container.id = 'font-size-controls';
        container.className = 'font-size-controls';
        container.setAttribute('aria-label', 'Font Size Controls');

        var btnDecrease = document.createElement('button');
        btnDecrease.className = 'font-size-btn';
        btnDecrease.title = 'Decrease Font Size';
        btnDecrease.innerText = 'A−';

        var btnReset = document.createElement('button');
        btnReset.className = 'font-size-btn font-size-reset';
        btnReset.title = 'Reset Font Size';
        btnReset.innerText = 'A';

        var btnIncrease = document.createElement('button');
        btnIncrease.className = 'font-size-btn';
        btnIncrease.title = 'Increase Font Size';
        btnIncrease.innerText = 'A+';

        var currentSize = parseFloat(localStorage.getItem('sip2json_font_scale')) || 1.0;

        function applyScale(scale) {
            currentSize = Math.min(Math.max(scale, 0.8), 1.35);
            document.documentElement.style.setProperty('--doc-font-scale', currentSize.toString());
            document.documentElement.style.fontSize = (currentSize * 100) + '%';
            localStorage.setItem('sip2json_font_scale', currentSize.toFixed(2));
        }

        btnDecrease.onclick = function() { applyScale(currentSize - 0.05); };
        btnReset.onclick = function() { applyScale(1.0); };
        btnIncrease.onclick = function() { applyScale(currentSize + 0.05); };

        container.appendChild(btnDecrease);
        container.appendChild(btnReset);
        container.appendChild(btnIncrease);

        // Insert before search or at end of header
        var search = document.querySelector('.md-header__source') || document.querySelector('.md-header__option');
        if (search && search.parentNode) {
            search.parentNode.insertBefore(container, search);
        } else {
            header.appendChild(container);
        }

        // Apply saved scale on load
        if (currentSize !== 1.0) {
            applyScale(currentSize);
        }
    }

    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', initFontSizeControl);
    } else {
        initFontSizeControl();
    }

    // Support instant navigation in Material for MkDocs
    if (window.document$ && typeof window.document$.subscribe === 'function') {
        window.document$.subscribe(initFontSizeControl);
    }
})();
