using UnityEngine;
using UnityEditor;
using System.IO;

public class CreateSpritesOnScene : EditorWindow
{
    [MenuItem("Tools/Create 600 Sprites")]
    static void CreateSprites()
    {
        // НАСТРОЙКИ (измените под себя)
        string sourceFolder = "Assets/000"; // путь к папке со спрайтами
        float startX = 0f;                      // начальная позиция X первого спрайта
        float startY = 0f;                      // начальная позиция Y
        float spacing = 0.2f;                    // дополнительный отступ между спрайтами

        // Проверяем, существует ли папка
        if (!AssetDatabase.IsValidFolder(sourceFolder))
        {
            Debug.LogError("Папка не найдена: " + sourceFolder);
            return;
        }

        // Получаем все ассеты в папке (включая подпапки)
        string[] guids = AssetDatabase.FindAssets("t:Sprite", new[] { sourceFolder });
        if (guids.Length == 0)
        {
            Debug.LogError("В папке не найдено спрайтов.");
            return;
        }

        // Начинаем создавать объекты
        int count = 0;
        float currentX = startX;
        
        foreach (string guid in guids)
        {
            // Получаем путь к ассету и сам спрайт
            string path = AssetDatabase.GUIDToAssetPath(guid);
            Sprite sprite = AssetDatabase.LoadAssetAtPath<Sprite>(path);
            if (sprite == null) continue;

            // Получаем имя подпапки и имя файла без расширения
            string folderName = Path.GetFileName(Path.GetDirectoryName(path));
            string fileName = Path.GetFileNameWithoutExtension(path);
            
            // Новое имя для GameObject (то, что вы видите в Hierarchy)
            string gameObjectName = $"{folderName}_{fileName}";
            
            // Создаём GameObject с SpriteRenderer
            GameObject go = new GameObject(gameObjectName);
            SpriteRenderer sr = go.AddComponent<SpriteRenderer>();
            
            // Создаем копию спрайта с новым именем
            string newSpritePath = Path.Combine(sourceFolder, $"{folderName}_{fileName}.asset");
            
            // Проверяем, не существует ли уже такой спрайт
            Sprite existingSprite = AssetDatabase.LoadAssetAtPath<Sprite>(newSpritePath);
            if (existingSprite != null)
            {
                sr.sprite = existingSprite;
            }
            else
            {
                // Создаем новый спрайт как отдельный ассет
                Sprite newSprite = Object.Instantiate(sprite);
                newSprite.name = $"{folderName}_{fileName}";
                
                // Сохраняем новый спрайт в папку
                AssetDatabase.CreateAsset(newSprite, newSpritePath);
                AssetDatabase.SaveAssets();
                AssetDatabase.Refresh();
                
                sr.sprite = newSprite;
            }

            // Получаем ширину спрайта в мировых единицах
            float spriteWidth = sprite.bounds.size.x;
            
            // Устанавливаем позицию
            go.transform.position = new Vector3(currentX + spriteWidth/2, startY, 0);

            // Увеличиваем текущую позицию X на ширину спрайта + отступ
            currentX += spriteWidth + spacing;

            // Регистрируем создание для возможности Undo (Ctrl+Z)
            Undo.RegisterCreatedObjectUndo(go, "Create " + fileName);

            count++;
        }

        Debug.Log($"Готово! Создано {count} спрайтов на сцене с новыми именами.");
    }
}