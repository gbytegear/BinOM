document.querySelectorAll('button[data-scroll-to^="#"]').forEach(anchor => {
    anchor.addEventListener('click', function (e) {
        e.preventDefault();

        document.querySelector(this.getAttribute('data-scroll-to')).scrollIntoView({
            behavior: 'smooth'
        });
    });
});