using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using UnityEditor;
using UnityEngine;



public class JournalEntry
{
    
    public enum BlockType
    {
        title = 0,
        maintext = 1,
        //imagetext = 3
    }
    public string title;
    //public List<string> mainText = new List<string>();
    //public List<Texture2D> images = new List<Texture2D>();//the image for the journal entry
    public List<TextAndImage> textsAndImages = new List<TextAndImage>();
    public List<BlockType> BlockOrder = new List<BlockType>();
    public bool display = true;
    public int startOnPage = -1;

    public JournalEntry() { }

    public JournalEntry(JournalEntry j)
    {
        title = j.title;
        foreach(TextAndImage t in j.textsAndImages)
        {
            textsAndImages.Add(new TextAndImage(t));
        }
        foreach(BlockType b in j.BlockOrder)
        {
            BlockOrder.Add(b);
        }
        display = j.display;
        startOnPage = j.startOnPage;
    }
}


//replaces images and text in journal entry, combined both into one, counts as a block, but has images inside it
//[System.Serializable]
public class TextAndImage
{
    public string text;
    public Img image;

    public TextAndImage() { }

    public TextAndImage(TextAndImage t)
    {
        text = t.text;
        if(t.image != null)
        {
            image = new Img(t.image.img, t.image.index);
        }     
    }
}

//[System.Serializable]
public class Img
{
    public Texture2D img;
    public int index;//where in the corresponding string it should be placed
    public Img(Texture2D t, int i)
    {
        img = t;
        index = i;
    }
}

/// <summary>
/// Parses and stores all journal entries that the writers create
/// Creates the corresponding UI object
/// </summary>
public class JournalManager : MonoBehaviour
{
    public static JournalManager instance;

    public MenuDivision MenuSettings;

    public List<List<JournalEntry>> AllEntries = new List<List<JournalEntry>>();
    /*
    public List<JournalEntry> TableOfContentsEntries = new List<JournalEntry>();
    public List<JournalEntry> PlayerEntries = new List<JournalEntry>();
    public List<JournalEntry> ParentEntries = new List<JournalEntry>();
    public List<JournalEntry> EnvironmentEntries = new List<JournalEntry>();
    public List<JournalEntry> DreamEntries = new List<JournalEntry>();
    */


    /*REMOVED 9-20-2019, adding addtional menu sections
    public TextAsset PlayerEntriesFile;
    public TextAsset ParentEntriesFile;
    public TextAsset EnvironmentEntriesFile;
    public TextAsset DreamEntriesFile;\
    */

    //ADDED 9-20-2019
    public TextAsset[] textAssets;
    //

    private string[] tokens = new string[] { "[title]", "[block]"/*, "[image]"*/};
    private string[] imgTokens = new string[] { "<image>", "</image>" };


    // actual initialization
    void Start()
    {
        //TODO: Remove instances most likely
        if (instance == null)
        {
            instance = this;
        }

        textAssets = MenuSettings.textAssets;

        /*REMOVED 9-20-2019, adding addtional menu sections
        ParseJournalFile(PlayerEntriesFile, PlayerEntries);
        ParseJournalFile(ParentEntriesFile, ParentEntries);
        ParseJournalFile(EnvironmentEntriesFile, EnvironmentEntries);
        ParseJournalFile(DreamEntriesFile, DreamEntries);
        */
        for(int i = 0; i < textAssets.Length; i++)
        {
            List<JournalEntry> temp = new List<JournalEntry>();
            ParseJournalFile(textAssets[i], temp);
            AllEntries.Add(temp);
        }

        if(MenuSettings.isOptionsMenu)
        {
            GameObject OptionUI = new GameObject("OptionsUI");
            OptionUI.transform.SetParent(this.transform, false);
            OptionsUIScript UIComp = OptionUI.AddComponent<OptionsUIScript>();
            UIComp.Settings = MenuSettings;
            UIComp.journal = this;

            MenuManager.instance.DivisionManagers.Add(this.gameObject);
        }
        else
        {
            //creating the UI object
            GameObject UI = new GameObject(name + "UI");
            UI.transform.SetParent(this.transform, false);
            JournalUI UIComponent = UI.AddComponent<JournalUI>();
            UIComponent.Settings = MenuSettings;
            UIComponent.journal = this;

            MenuManager.instance.DivisionManagers.Add(this.gameObject);
        }


        /*REMOVED 9-20-2019, adding additional menu sections
        AllEntries.Add(TableOfContentsEntries);//0
        AllEntries.Add(PlayerEntries);//1
        AllEntries.Add(ParentEntries);//2
        AllEntries.Add(EnvironmentEntries);//3
        AllEntries.Add(DreamEntries);//4
        */


        /*
        //making table of contents
        //every section is an entry, and every title is a block
        //temporary code
        JournalEntry players = new JournalEntry();
        players.title = "Player Entries";
        players.BlockOrder.Add(JournalEntry.BlockType.title);

        JournalEntry parents = new JournalEntry();
        parents.title = "Parent Entries";
        parents.BlockOrder.Add(JournalEntry.BlockType.title);

        JournalEntry environments = new JournalEntry();
        environments.title = "Environment Entries";
        environments.BlockOrder.Add(JournalEntry.BlockType.title);

        JournalEntry dreams = new JournalEntry();
        dreams.title = "Dream Entries";
        dreams.BlockOrder.Add(JournalEntry.BlockType.title);
        */

        /*REMOVED 9-20-2019, adding additional menu sections
        TableOfContentsEntries.Add(players);
        TableOfContentsEntries.Add(parents);
        TableOfContentsEntries.Add(environments);
        TableOfContentsEntries.Add(dreams);
        */
    }

    // Update is called once per frame
    void Update()
    {

    }

    /// <summary>
    /// Returns the first JournalEntry containing the 'title' string in the title. 
    /// Returns null if no matching JournalEntry is found. 
    /// Use with JournalUI.AddEntry(), checks for null in AddEntry()
    /// RIGHT NOW: Can't use to find JournalEntry of things already placed because sometimes entries are altered
    /// </summary>
    /// <param name="title">Entry title</param>
    /// <returns>JournalEntry with title containing string 'title' or null</returns>
    public JournalEntry FindEntryFromTitle(string title)
    {
        foreach(List<JournalEntry> list in AllEntries)
        {
            for(int i = 0; i < list.Count; i++)
            {
                if(list[i].title.Equals(title))
                {
                    return list[i];
                }
            }
        }
        return null;
    }

    /// <summary>
    /// Parses a journal file and stores entries
    /// </summary>
    /// <param name="text">TextAsset to parse</param>
    /// <param name="entriesToFill">List to fill with entries</param>
    private void ParseJournalFile(TextAsset text, List<JournalEntry> entriesToFill)
    {//for parsing a journal file, each section of the journal has its own file
        if(text != null)
        {
            
            //split up each entry
            string[] entries = text.text.Split(new[] { "[entry]" }, StringSplitOptions.None);
            
            for(int i = 0; i < entries.Length; i++)
            {//for each individual entry
                bool display = true;
                int num = -1;

                if (entries[i] == "")
                {
                    continue;
                }
                entries[i] = entries[i].Trim();//remove all newlines and whitespace at the end and front
                //entries[i] = entries[i].Replace(System.Environment.NewLine, "");
                //split each entry into all the blocks

                string[] blocks = entries[i].Split(tokens, StringSplitOptions.None);
                for(int f = 0; f < blocks.Length; f++)
                {
                    if (blocks[f].Contains("[display false]") && blocks[f].Contains("[page"))
                    {//split display false and the page into separate blocks
                        int ind = blocks[f].IndexOf("[page") > blocks[f].IndexOf("[display false]") ? blocks[f].IndexOf("[page") : blocks[f].IndexOf("[display false]");

                        string b = blocks[f].Substring(0, ind);
                        string p = blocks[f].Substring(ind);
                        blocks[f] = b;

                        List<string> add = new List<string>(blocks);
                        add.Insert(f + 1, p);
                        blocks = add.ToArray();
                    }
                    if(blocks[f] == "")
                    {
                        List<string> temp = new List<string>(blocks);
                        temp.RemoveAt(f);
                        blocks = temp.ToArray();
                    }
                    if(blocks[f].Contains("[display false]"))
                    {
                        string toRemove = blocks[f];
                        display = false;
                        List<string> temp = new List<string>(blocks);
                        temp.RemoveAt(f);
                        blocks = temp.ToArray();
                        entries[i] = entries[i].Replace(toRemove, "");
                        f--;
                        continue;
                    }
                    if(blocks[f].Contains("[page"))
                    {
                        string toRemove = blocks[f];

                        string numString = Regex.Match(toRemove, @"\d+").Value;
                        num = Int32.Parse(numString);

                        List<string> temp = new List<string>(blocks);
                        temp.RemoveAt(f);
                        blocks = temp.ToArray();
                        entries[i] = entries[i].Replace(toRemove, "");
                        f--;
                        continue;
                    }
                    blocks[f] = blocks[f].Trim();//remove newline and whitespace at beginning and end of each block

                }
                entriesToFill.Add(findOrder(entries[i], blocks, display, num));

            }
        }
    }

    /// <summary>
    /// Find the order of the entries in provided string and creates the entry
    /// </summary>
    /// <param name="entry">Entry string</param>
    /// <param name="blocks">Array with strings of each block</param>
    /// <param name="display">Whether this entry should display on start or not</param>
    /// <param name="num">What page this page starts on, -1 if no startpage</param>
    /// <returns>Generated JournalEntry</returns>
    private JournalEntry findOrder(string entry, string[] blocks, bool display, int num)
    {//entry is the entry with all the tokens in there ie: [title], etc
        /*for (int k = 0; k < blocks.Length; k++)
        {
            Debug.Log(blocks[k]);
        }
        Debug.Log(entry);*/

        JournalEntry journalEntry = new JournalEntry();
        //journalEntry.mainText = new List<string>();
        //journalEntry.images = new List<Texture2D>();
        //journalEntry.BlockOrder = new List<JournalEntry.BlockType>();

        if (!display)
        {
            journalEntry.display = false;
        }
        if(num != -1)
        {
            journalEntry.startOnPage = num - 1;//page 1 is really 0 index
        }


        List<int> TitleIndices = IndicesOf(tokens[0], entry);
        List<int> BlockIndices = IndicesOf(tokens[1], entry);
        //List<int> ImageIndices = IndicesOf(tokens[2], entry);



        List<int> IndicesCombined = new List<int>();//list of the indices combined to use to fill the actual values in journal entry later
        while(TitleIndices.Count != 0 || BlockIndices.Count != 0 /*|| ImageIndices.Count != 0*/)
        {//to order the BlockOrder list based on the indices of each block
            int lowestInd = -1;
            for(int t = 0; t < TitleIndices.Count; t++)
            {
                if(lowestInd == -1)
                {
                    lowestInd = TitleIndices[t];
                }
                else if(TitleIndices[t] < lowestInd)
                {
                    lowestInd = TitleIndices[t];
                }
            }
            for(int b = 0; b < BlockIndices.Count; b++)
            {
                if(lowestInd == -1)
                {
                    lowestInd = BlockIndices[b];
                }
                else if(BlockIndices[b] < lowestInd)
                {
                    lowestInd = BlockIndices[b];
                }
            }


            if(TitleIndices.Contains(lowestInd))
            {
                journalEntry.BlockOrder.Add(JournalEntry.BlockType.title);
                TitleIndices.Remove(lowestInd);
            }
            else if(BlockIndices.Contains(lowestInd))
            {
                journalEntry.BlockOrder.Add(JournalEntry.BlockType.maintext);
                BlockIndices.Remove(lowestInd);
            }
           /* else if(ImageIndices.Contains(lowestInd))
            {
                journalEntry.BlockOrder.Add(JournalEntry.BlockType.image);
                ImageIndices.Remove(lowestInd);
            }*/

            IndicesCombined.Add(lowestInd);
        }//blockorder now ordered correctly


        for(int i = 0; i < journalEntry.BlockOrder.Count; i++)
        {//filling the values in the actual journal entry
            string temp = blocks[i];

            if (journalEntry.BlockOrder[i] == JournalEntry.BlockType.title)
            {
                journalEntry.title = temp;
            }
            else if (journalEntry.BlockOrder[i] == JournalEntry.BlockType.maintext)
            {
                TextAndImage toAdd = new TextAndImage();
                
                if (temp.Contains("<image>") && temp.Contains("</image>"))
                {
                   // int firstIndex = 1;

                    string[] withImages = temp.Split(imgTokens, StringSplitOptions.None);
                    string imageFilename = withImages[1];

                    imageFilename = imageFilename.Trim();
                    #if UNITY_EDITOR
                    Texture2D texture = (Texture2D)AssetDatabase.LoadAssetAtPath(imageFilename, typeof(Texture2D));
                    #else
                    Texture2D texture = (Texture2D)Resources.Load(imageFilename, typeof(Texture2D));
                    #endif
                    Debug.Log(imageFilename);
                    if (texture == null)
                    {
                        Debug.Log("Journal image " + imageFilename + " could not be loaded.");                      
                        
                    }
                    else
                    {
                        Img IGGY = new Img(texture, withImages[0].Length);

                        toAdd.image = IGGY;
                    }

                    //}

                    temp = "";
                    for(int n = 0; n < withImages.Length; n++)
                    {
                        if (n == 1) continue;
                        temp += withImages[n];
                    }

                }
                
                toAdd.text = temp;
                journalEntry.textsAndImages.Add(toAdd);
            }

        }
        return journalEntry;
    }

    private List<int> IndicesOf(string stringToFind, string mainString)
    {//returns the starting indices of all the occurences of a specified string as a list
        List<int> indices = new List<int>();

        int index = mainString.IndexOf(stringToFind);
        int length = stringToFind.Length;
        while (index != -1)
        {
            indices.Add(index);

            //replacing one of the chars so it wont be detected on the next pass
            char[] charArr = mainString.ToCharArray();
            charArr[index] = '~';
            mainString = new string(charArr);

            index = mainString.IndexOf(stringToFind);
        }
        //now indices will have the indices of where the tokens start
        return indices;
    }
}
