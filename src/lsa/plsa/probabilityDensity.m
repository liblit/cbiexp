function probabilityDensity(Pw_z, curdir)
    imagesc(Pw_z); colormap(gray); colorbar;
    set(gca, 'XTick', [1:size(Pw_z,2)]);
    xlabel('Aspect'); ylabel('Feature'); title('Pw|z density');
    outputfile = [curdir '/feature_probabilities'];
    print(gcf, '-djpeg', outputfile);
return;
